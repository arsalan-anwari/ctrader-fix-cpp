#pragma once

#include <stdio.h>
#include <string>

#include <bitset>

#include "types/decode.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"

#include "settings.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::types::symbol;
using namespace ctrader::tools;


namespace internal {

    using namespace ctrader::data;
    using namespace ctrader::settings;

    #define __DECODE_GEN_PATTERN(pattern) __SETTINGS_SOH #pattern

    inline __attribute__((always_inline))
    u32 find_pattern_32a(const char* text, const char* pattern){
        std::string _text(text, 32);
        std::string _pattern(pattern, 4);
        auto pos = _text.find(_pattern);
        if (pos == std::string::npos){ return 0U; }
        return pos + 1;
    };

    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    decode_metadata get_message_meta(const char* data, const u32 msg_seq_num_digit_size){

        auto msg_size_info = numbers::to_num_estimate<u32, 4>(data+12);

        const u32 header_size = ( 
            9U + // |35=X|34=
            msg_seq_num_digit_size + // {0:1-18}
            49U + // |49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=
            broker_settings::SenderCompID.size() // demo.icmarkets.8536054
        );

        const u32 size = msg_size_info.value - header_size;

        const u32 offset = (
            12U + // 8=FIX.4.4|9=
            msg_size_info.digit_count + // {0:1-4}
            header_size 
        );
        
        
        return decode_metadata{ offset, size };
    };


} // internal

namespace algorithms {

    namespace market_orders{

        // // |279=0|269=1|278=2291668712|55=1|270=1.08754|271=3000000
        // inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
        // void create_insert_order_inc(const char* chunk, decode_data<DATA_TYPE::MARKET_DATA>& entry, u16 start, u16 end){
        //     entry.UpdateAction = UPDATE_ACTION::NEW;
        //     entry.EntryType = ENTRY_TYPE_LOOKUP[ chunk[11] - '0' ];
        //     entry.EntryId = numbers::to_num<i64, 10>(chunk+17);
 

        // }
    }

    template<DECODE_TYPE T, typename... CONTAINER_TYPE>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void parsing_algorithm( const char* data, const u32 size, const u32 num_entries, CONTAINER_TYPE&& ... containers  );

    template<> void parsing_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>
    (   const char* data, const u32 size, const u32 num_entries,
        message_container<DATA_TYPE::MARKET_DATA>& market_data,
        market_index_container& market_indices,
        market_index_filter& market_insert_index_filter,
        market_index_filter& market_remove_index_filter
    ){
    
        static const u32 base_skip_size_lookup[4] = {50U, 4U, 26U, 4U};

        // Precalculate first entry (always starts at i=0)
        u32 message_type = ( data[5] - '0' );
        market_indices[message_type][0].begin = 0;
        u32 absolute_offset = base_skip_size_lookup[message_type];
        u32 old_message_type = message_type;
        u32 mi_idx = 1;

        // Handle unrolled vectorized market_indices calculations
        for(u32 i=0; i<25; i++){
            // Offset calculations
            u32 is_vectorizable = numbers::op::lte( (absolute_offset + 32), size);

            u32 chunk_start = absolute_offset * is_vectorizable;
            
            u32 search_idx = internal::find_pattern_32a(data+chunk_start, "|279");
            u32 idx_found = numbers::op::gte( search_idx, 0);

            u32 relative_offset = ((search_idx - 1U) * (idx_found & is_vectorizable));
            u32 message_type_idx = ((chunk_start * idx_found) + relative_offset) * is_vectorizable;
            u32 new_message_type = ((( data[message_type_idx + 5U] - '0' ) + ( (idx_found ^ 1) | (is_vectorizable ^ 1) )));

            u32 skip_size = (base_skip_size_lookup[new_message_type] + relative_offset) * is_vectorizable;

            u32 insert_value = absolute_offset + relative_offset;
            absolute_offset += skip_size;

            // Insert calculations
            u32 new_mi_idx = mi_idx;
            u32 old_mi_idx = (new_mi_idx-1);

            market_indices[ new_message_type ][ new_mi_idx ].begin = insert_value; 
            market_indices[ old_message_type ][ old_mi_idx ].end = insert_value;

            old_message_type = new_message_type;
            mi_idx += (idx_found & is_vectorizable);
        }

        // Handle last part of market_indices
        u32 end_size = size - absolute_offset;
        u32 padding_size = 32U - end_size;

        u32 search_idx = internal::find_pattern_32a(data+(absolute_offset-padding_size), "|279");
        u32 idx_found = numbers::op::gte( search_idx, 0);
  
        u32 relative_offset = (absolute_offset + ((search_idx - 1U) - padding_size)) * idx_found;
        u32 new_message_type = ( data[relative_offset + 5] - '0' ) + (idx_found ^ 1U);
        
        u32 new_mi_idx = mi_idx;
        u32 old_mi_idx = (new_mi_idx-1);
        market_indices[ new_message_type ][ new_mi_idx ].begin = relative_offset; 
        market_indices[ old_message_type ][ old_mi_idx ].end = relative_offset;
        market_indices[ new_message_type ][ new_mi_idx ].end = size;

        // Filter out found index ranges and stores in temporary array
        u32 market_insert_count = 0, market_remove_count = 0, market_insert_idx = 0, market_remove_idx = 0;

        for (u32 i = 0; i < num_entries; i++){
            i32 begin_insert = market_indices[0][i].begin;
            i32 end_insert = market_indices[0][i].end;
            i32 begin_remove = market_indices[2][i].begin;
            i32 end_remove = market_indices[2][i].end;

            u32 result_state_insert = numbers::op::ne(begin_insert, end_insert); 
            u32 result_state_remove = numbers::op::ne(begin_remove, end_remove); 

            market_insert_idx += result_state_insert;
            market_insert_count += result_state_insert;
            market_remove_idx += result_state_remove;
            market_remove_count += result_state_remove;

            market_insert_index_filter.data[ market_insert_idx * result_state_insert ] = i;
            market_remove_index_filter.data[ market_remove_idx * result_state_remove ] = i;
        }

        market_insert_index_filter.data_len = market_insert_count;
        market_remove_index_filter.data_len = market_remove_count;
        // u8 market_data_idx = 0;
        // for (u8 i=1; i < market_insert_count+1; i++){
        //     auto filter_idx = market_insert_index_filter.data[i];
            
        //     decode_data<DATA_TYPE::MARKET_DATA>& entry = market_data[market_data_idx++];
        //     market_orders::create_insert_order_inc(
        //         data, entry, 
        //         market_indices[0][filter_idx].begin, 
        //         market_indices[0][filter_idx].end
        //     );
        // }

    };

} // parsers


struct Decoder{
    Decoder(){};

    template<DECODE_TYPE T>
    inline __attribute__((always_inline))
    u32 decode(const char* data, const u8 msg_seq_num_digit_size){ 
        if constexpr (T == DECODE_TYPE::MARKET_DATA_INCREMENTAL){
            decode_metadata meta = internal::get_message_meta(data, msg_seq_num_digit_size);
            auto entries = numbers::to_num_estimate<u32, 2>(data+meta.offset+5);

            algorithms::parsing_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>(
                data+meta.offset+5+entries.digit_count, meta.size-(5+entries.digit_count), entries.value, 
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