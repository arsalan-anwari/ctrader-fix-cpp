#pragma once

#include <stdio.h>

#include <cstring>
#include <string>

#include "types/decode.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"

#include "global.hpp"
#include "settings.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::tools;

namespace internal {

    using namespace ctrader::data;
    using namespace ctrader::settings;
    using namespace ctrader::parser;

    #define __DECODE_GEN_PATTERN(pattern) __SETTINGS_SOH #pattern

    inline __attribute__((always_inline))
    uint8_t find_pattern_32a(const char* text, const char* pattern){
        std::string _text(text, 32);
        std::string _pattern(pattern, 4);
        auto pos = _text.find(_pattern);
        if (pos == std::string::npos){ return 0; }
        return static_cast<uint8_t> (pos + 1);
    };

    inline __attribute__((always_inline))
    uint8_t find_pattern_normal(const char* text, const char* pattern, uint8_t size){
        std::string _text(text, size);
        std::string _pattern(pattern, 4);
        auto pos = _text.find(_pattern);
        if (pos == std::string::npos){ return 0; }
        return static_cast<uint8_t> (pos + 1);
    };
    

    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    decode_metadata get_message_meta(const char* data){

        auto msg_size_info = numbers::to_num_estimate<uint16_t, 4>(data+12);

        const uint16_t header_size = ( 
            9 + // |35=X|34=
            global::MsgSeqNum::msg_seq_num_digit_size + // {0:1-18}
            49 + // |49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=
            broker_settings::SenderCompID.size() // demo.icmarkets.8536054
        );

        const uint16_t size = msg_size_info.value - header_size;

        const uint16_t offset = (
            12 + // 8=FIX.4.4|9=
            msg_size_info.digit_count + // {0:1-4}
            header_size 
        );
        
        
        return decode_metadata{ offset, size };
    };


} // internal

namespace algorithms {

    namespace market_orders{
        using namespace ctrader::types::symbol;

        inline __attribute__((always_inline))
        void create_insert_order_inc(const char* data, decode_data<DATA_TYPE::MARKET_DATA>& entry, uint16_t start, uint16_t end){
            entry.UpdateAction = UPDATE_ACTION::NEW;
            entry.EntryType = ENTRY_TYPE_LOOKUP[ static_cast<uint8_t>( data[start+11] ) ];
            entry.EntryId = numbers::to_num<int64_t, 10>(data+start+17);
            entry.SymbolId = SYMBOL_LOOKUP[ static_cast<uint8_t>( data[start+31]) ];
        }
    }

    template<DECODE_TYPE T, typename... CONTAINER_TYPE>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void parsing_algorithm( const char* data, uint16_t size, CONTAINER_TYPE&& ... containers  ){};

    template<> void parsing_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>
    (   const char* data, uint16_t size, uint16_t& num_entries,
        message_container<DATA_TYPE::MARKET_DATA>& market_data,
        market_index_container& market_indices,
        market_index_filter& market_insert_index_filter,
        market_index_filter& market_remove_index_filter
    ){
        //printf("ik word aangeroepen!");
        const uint8_t base_skip_size_lookup[4] = {50, 4, 26, 4};

        // Precalculate first entry (always starts at i=0)
        uint8_t message_type = static_cast<uint8_t>( data[5] - '0' );
        market_indices[message_type][0].begin = 0;
        uint16_t absolute_offset = base_skip_size_lookup[message_type];
        uint8_t old_message_type = message_type;
        uint16_t mi_idx = 1;

        // Handle vectorized market_indices calculations
        for(uint16_t i=0; i<num_entries; i++){
            // Offset calculations
            bool is_vectorizable = (absolute_offset + 32) < size;

            uint16_t chunk_start = absolute_offset * is_vectorizable;
            
            uint8_t search_idx = internal::find_pattern_32a(data+chunk_start, "|279");
            bool idx_found = search_idx > 0;

            uint8_t relative_offset = ((search_idx - 1) * (idx_found && is_vectorizable));
            uint16_t message_type_idx = ((chunk_start * idx_found) + relative_offset) * is_vectorizable;
            uint8_t new_message_type = ((( data[message_type_idx + 5] - '0' ) + ( (!idx_found) || (!is_vectorizable) )));

            uint8_t skip_size = (base_skip_size_lookup[new_message_type] + relative_offset) * is_vectorizable;

            uint16_t insert_value = absolute_offset + relative_offset;
            absolute_offset += skip_size;

            // Insert calculations
            uint16_t new_mi_idx = mi_idx;
            uint16_t old_mi_idx = (new_mi_idx-1);

            market_indices[ new_message_type ][ new_mi_idx ].begin = insert_value; 
            market_indices[ old_message_type ][ old_mi_idx ].end = insert_value;

            old_message_type = new_message_type;
            mi_idx += (idx_found && is_vectorizable);
        }

        // Handle last part of market_indices
        uint8_t end_size = size - absolute_offset;
        uint8_t search_idx = internal::find_pattern_normal(data+absolute_offset, "|279", end_size);
        bool idx_found = search_idx > 0;
        
        uint16_t relative_offset = (absolute_offset + (search_idx - 1)) * idx_found;
        uint8_t new_message_type = ( data[relative_offset + 5] - '0' ) + (!idx_found);

        uint16_t new_mi_idx = mi_idx;
        uint16_t old_mi_idx = (new_mi_idx-1);
        market_indices[ new_message_type ][ new_mi_idx ].begin = relative_offset; 
        market_indices[ old_message_type ][ old_mi_idx ].end = relative_offset;
        market_indices[ new_message_type ][ new_mi_idx ].end = size;

        // Filter out found index ranges and stores in temporary array
        uint8_t market_insert_count, market_remove_count, market_insert_idx, market_remove_idx = 0;

        for (uint8_t i = 0; i < num_entries + 1; i++){
            auto begin_insert = market_indices[0][i].begin;
            auto end_insert = market_indices[0][i].end;
            auto begin_remove = market_indices[2][i].begin;
            auto end_remove = market_indices[2][i].end;

            bool result_state_insert = (begin_insert > -1 || end_insert > -1) && (begin_insert != end_insert);
            bool result_state_remove = (begin_remove > -1 || end_remove > -1) && (begin_remove != end_remove);

            market_insert_idx += result_state_insert;
            market_insert_count += result_state_insert;
            market_remove_idx += result_state_remove;
            market_remove_count += result_state_remove;

            market_insert_index_filter.data[ market_insert_idx * result_state_insert ] = i;
            market_remove_index_filter.data[ market_remove_idx * result_state_remove ] = i;
        }

        uint8_t market_data_idx = 0;
        for (uint8_t i=1; i < market_insert_count+1; i++){
            auto filter_idx = market_insert_index_filter.data[i];
            
            decode_data<DATA_TYPE::MARKET_DATA>& entry = market_data[market_data_idx++];
            market_orders::create_insert_order_inc(
                data, entry, 
                market_indices[0][filter_idx].begin, 
                market_indices[0][filter_idx].end
            );
        }

    };

} // parsers


struct Decoder{
    Decoder(){};

    template<DECODE_TYPE T>
    inline __attribute__((always_inline))
    uint8_t decode(const char* data){ 
        if constexpr (T == DECODE_TYPE::MARKET_DATA_INCREMENTAL){
            decode_metadata meta = internal::get_message_meta(data);
            auto entries = numbers::to_num_estimate<uint16_t, 3>(data+meta.offset+5);

            algorithms::parsing_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>(
                data+meta.offset+5+entries.digit_count, meta.size-(5+entries.digit_count),entries.value, 
                market_data, market_incremental_indices, market_incremental_insert_index_filter, market_incremental_remove_index_filter
            ); 

            return entries.value;
        } else { return 0U; }

    };

public:
    message_container<DATA_TYPE::MARKET_DATA> market_data;
    market_index_container market_incremental_indices;
    market_index_filter market_incremental_insert_index_filter;
    market_index_filter market_incremental_remove_index_filter;
};


} // ctrader::parser

// /**
// Received:  8=FIX.4.4|9=633
// |35=W|34=2|49=cServer|50=QUOTE|52=20230124-13:30:46.025|56=demo.icmarkets.8536054
// |55=1
// |268=12
// |269=1|270=1.08755|271=1500000|278=2291666392
// |269=1|270=1.08765|271=20000000|278=2291666393
// |269=1|270=1.08753|271=100000|278=2291666394
// |269=1|270=1.08759|271=10000000|278=2291661810
// |269=1|270=1.08752|271=50000|278=2291666388
// |269=1|270=1.08757|271=5000000|278=2291666389
// |269=1|270=1.08756|271=3000000|278=2291666390
// |269=0|270=1.08747|271=10000000|278=2291666384
// |269=0|270=1.08741|271=20000000|278=2291666385
// |269=0|270=1.0875x|271=3000000|278=2291666386
// |269=0|270=1.08752|271=1650000|278=2291666387
// |269=0|270=1.08749|271=5000000|278=2291666381
// |10=091|

// Received:  
// 8=FIX.4.4|9=1128
// |35=X|34=3|49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=demo.icmarkets.8536054
// |268=25
// |279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000
// |279=0|269=1|278=2291667249|55=1|270=1.08757|271=10000000
// |279=0|269=1|278=2291667250|55=1|270=1.08752|271=1500000
// |279=0|269=1|278=2291667252|55=1|270=1.08749|271=100000
// |279=0|269=1|278=2291667253|55=1|270=1.08753|271=3000000
// |279=0|269=1|278=2291667254|55=1|270=1.08748|271=50000
// |279=0|269=1|278=2291667255|55=1|270=1.08764|271=20000000
// |279=2|278=2291666392|55=1
// |279=2|278=2291666393|55=1
// |279=2|278=2291666394|55=1
// |279=2|278=2291661810|55=1
// |279=2|278=2291666388|55=1
// |279=2|278=2291666389|55=1
// |279=2|278=2291666390|55=1
// |279=0|269=0|278=2291667242|55=1|270=1.08747|271=1500000
// |279=0|269=0|278=2291667243|55=1|270=1.08742|271=10000000
// |279=0|269=0|278=2291667244|55=1|270=1.08748|271=150000
// |279=0|269=0|278=2291667245|55=1|270=1.08745|271=3000000
// |279=0|269=0|278=2291667246|55=1|270=1.08744|271=5000000
// |279=0|269=0|278=2291667247|55=1|270=1.08737|271=20000000
// |279=2|278=2291666381|55=1
// |279=2|278=2291666384|55=1
// |279=2|278=2291666385|55=1
// |279=2|278=2291666386|55=1
// |279=2|278=2291666387|55=1
// |10=244|

// 8=FIX.4.4|9=855|35=X|34=4|49=cServer|50=QUOTE|52=20230124-13:30:46.692|56=demo.icmarkets.8536054
// |268=18
// |279=0|269=1|278=2291668712|55=1|270=1.08754|271=3000000
// |279=0|269=1|278=2291668713|55=1|270=1.08755|271=5000000
// |279=0|269=1|278=2291668705|55=1|270=1.08772|271=50000000
// |279=0|269=1|278=2291668706|55=1|270=1.08753|271=1500000
// |279=0|269=1|278=2291668707|55=1|270=1.08758|271=10000000
// |279=0|269=1|278=2291668709|55=1|270=1.08749|271=50000
// |279=0|269=1|278=2291668711|55=1|270=1.0875|271=100000
// |279=2|278=2291667248|55=1
// |279=2|278=2291667249|55=1
// |279=2|278=2291667250|55=1
// |279=2|278=2291667252|55=1
// |279=2|278=2291667253|55=1
// |279=2|278=2291667254|55=1
// |279=0|269=0|278=2291668699|55=1|270=1.08749|271=150000
// |279=0|269=0|278=2291668700|55=1|270=1.08727|271=50000000
// |279=0|269=0|278=2291668703|55=1|270=1.08736|271=20000000
// |279=2|278=2291667244|55=1|279=2|278=2291667247|55=1
// |10=105|
// */