#pragma once

#include <stdio.h>

#include "types/decode.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"
#include "settings.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::types::symbol;

namespace {

    #define __DECODE_GEN_PATTERN(pattern) __SETTINGS_SOH_STR #pattern

    #define __DECODE_CASE(TYPE) \
        const u32 msg_seq_num_digit_size = memory::find<20>(data+12+msg_size_digit_size+9, __SETTINGS_SOH_CHAR);\
        const u32 header_size = get_message_header_size(msg_seq_num_digit_size);\
        const u32 data_size = numbers::to_num(data+12, msg_size_digit_size) - header_size - 7;\
        const u32 offset = ( 12U + msg_size_digit_size + header_size );\
        const u32 num_entries = numbers::to_num(data + offset + 5, memory::find<4>(data + offset + 5, __SETTINGS_SOH_CHAR));\
        decode_algorithm<TYPE>(data + offset + 7, data_size, num_entries);\
        return num_entries;\

    inline u32 get_message_header_size(const u32 msg_seq_num_digit_size){
        using namespace ctrader::settings;
        return ( 
                9U + // |35=X|34=
                msg_seq_num_digit_size + // {0:1-18}
                49U + // |49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=
                broker_settings::SenderCompID.size() // demo.icmarkets.8536054
        );
    }

    inline void create_insert_order_inc(const char* chunk, decode_data<DATA_TYPE::MARKET_DATA>& entry, u16 begin, u16 end){
        using namespace ctrader::tools;
        // example entry: |279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000

        entry.UpdateAction = UPDATE_ACTION::NEW;
        entry.EntryType = static_cast<ENTRY_TYPE>( chunk[11] - '0' );
        
        u16 entryLength;
        u16 offset = 17;

        entryLength = memory::find<12>(chunk+offset, __SETTINGS_SOH_CHAR);
        entry.EntryId = numbers::to_num<i64>(chunk+offset, entryLength);
        offset += entryLength + 4; // |55=

        entryLength = memory::find<10>(chunk+offset, __SETTINGS_SOH_CHAR);
        entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+offset, entryLength));
        offset += entryLength + 5; // |270= 

        entryLength = memory::find<16>(chunk+offset, __SETTINGS_SOH_CHAR);
        entry.EntryPrice.from_cstr(chunk+offset, entryLength);
        offset += entryLength + 5; // // |271=

        entryLength = (end - begin) - offset;
        entry.EntrySize = numbers::to_num<i64>(chunk+offset, entryLength);
    }

    inline void create_remove_order_inc(const char* chunk, decode_data<DATA_TYPE::MARKET_DATA>& entry, u16 begin, u16 end){
        using namespace ctrader::tools;
        // example entry: |279=2|278=2291666392|55=1

        entry.UpdateAction = UPDATE_ACTION::DELETE;
        entry.EntryType = ENTRY_TYPE::UNKNOWN;
        
        u16 entryLength;
        u16 offset = 11;

        entryLength = memory::find<12>(chunk+offset, __SETTINGS_SOH_CHAR);
        entry.EntryId = numbers::to_num<i64>(chunk+offset, entryLength);
        offset += entryLength + 4; // |55=

        entryLength = (end - begin) - offset;
        entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+offset, entryLength));
    }

}

struct Decoder{

    inline u32 decode_any(const char* data);

    template<DECODE_TYPE T>
    inline u32 decode(const char* data){
        using namespace ctrader::tools;
        const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH_CHAR);
        const u32 msg_seq_num_digit_size = memory::find<20>(data+12+msg_size_digit_size+9, __SETTINGS_SOH_CHAR);
        const u32 header_size = get_message_header_size(msg_seq_num_digit_size);
        const u32 data_size = numbers::to_num(data+12, msg_size_digit_size) - header_size - 7;
        const u32 offset = ( 12U + msg_size_digit_size + header_size );
        const u32 num_entries = numbers::to_num(data + offset + 5, memory::find<4>(data + offset + 5, __SETTINGS_SOH_CHAR));
        
        decode_algorithm<T>(data + offset + 7, data_size, num_entries);
        return num_entries;
    }    

private:

    template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
    inline void decode_algorithm(const char* data, const u32 data_size, const u32 num_entries);

public:
    u8 index_filter[128];
    u16 market_indices_begin[ 128 * 4 ];
    u16 market_indices_end[ 128 * 4 ];
    decode_data<DATA_TYPE::MARKET_DATA> market_data[128];

};

template<> void Decoder::decode_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>(const char* data, const u32 data_size, const u32 num_entries){
    using namespace ctrader::tools;
    static constexpr u8 base_skip_size_lookup[4] = {50U, 4U, 26U, 4U};
    static constexpr u16 market_indices_offset[4] = { 0, 128, 256, 384 }; 
    static constexpr char pattern[32] = __DECODE_GEN_PATTERN(279);

    // Precalculate first entry (always starts at i=0)
    u32 message_type = ( data[5] - '0' );
    market_indices_begin[market_indices_offset[message_type] + 0] = 0;
    u32 absolute_offset = base_skip_size_lookup[message_type];
    u32 old_message_type = message_type;
    u32 mi_idx = 1;

    // Handle vectorized market_indices calculations
    for(u32 i=0; i<num_entries; i++){
        // Offset calculations
        i32 is_vectorizable = numbers::op::lte( (absolute_offset + 32U), data_size);
        u32 is_vectorizable_mask = 0 - is_vectorizable;

        u32 chunk_start = absolute_offset & is_vectorizable_mask;
        u32 search_idx = memory::find_pattern_begin(data+chunk_start, pattern);

        i32 is_found = numbers::op::gte(search_idx, 0);
        u32 is_found_and_vectorizable_mask = 0 - (is_found & is_vectorizable);

        u32 relative_offset = (search_idx - 1U) & is_found_and_vectorizable_mask;
        u32 new_message_type = ((( data[chunk_start  + relative_offset + 5U] - '0' ) + ( (is_found ^ 1) | (is_vectorizable ^ 1) )));

        u32 insert_value = absolute_offset + relative_offset;
        u32 skip_size = (base_skip_size_lookup[new_message_type] + relative_offset) & is_vectorizable_mask;
        absolute_offset += skip_size;

        // Insert calculations
        u32 new_mi_idx = mi_idx;
        u32 old_mi_idx = (new_mi_idx-1);

        market_indices_begin[market_indices_offset[new_message_type] + new_mi_idx] = insert_value;
        market_indices_end[market_indices_offset[old_message_type] + old_mi_idx] = insert_value;

        old_message_type = new_message_type;
        mi_idx += (is_found & is_vectorizable);
    }

    // Handle last part of market_indices
    u32 end_size = data_size - absolute_offset;
    u32 padding_size = 32U - end_size;

    u32 search_idx = memory::find_pattern_begin(data+(absolute_offset-padding_size), pattern);

    i32 is_found = numbers::op::gte( search_idx, 0);
    u32 is_found_mask = 0 - is_found;

    u32 relative_offset = (absolute_offset + ((search_idx - 1U) - padding_size)) & is_found_mask;
    u32 new_message_type = ( data[relative_offset + 5] - '0' ) + (is_found ^ 1U);
    
    u32 new_mi_idx = mi_idx;
    u32 old_mi_idx = (new_mi_idx-1);
    
    market_indices_begin[market_indices_offset[new_message_type] + new_mi_idx] = relative_offset;
    market_indices_end[market_indices_offset[old_message_type] + old_mi_idx] = relative_offset;
    market_indices_end[market_indices_offset[new_message_type] + new_mi_idx] = data_size;

    // Filter out found index ranges and stores in temporary array
    u32 idx = 0, count_insert = 0, count_remove = 0;

    // idx_insert = 2 4 6 8 10 ... num_entries
    for (u32 i = 0; i < num_entries; i++){
        const i32 begin_insert = market_indices_begin[i];
        const i32 end_insert = market_indices_end[i];
        
        i32 result_state_insert = numbers::op::ne(begin_insert, end_insert); 
        u32 result_state_insert_mask = 0 - result_state_insert;

        idx += result_state_insert;
        count_insert += result_state_insert;

        index_filter[ idx & result_state_insert_mask ] = i;
    }

    // idx_remove = 1 3 5 7 9 ... num_entries
    idx = count_insert;
    for (u32 i = 0; i < num_entries; i++){
        const i32 begin_remove = market_indices_begin[256 + i];
        const i32 end_remove = market_indices_end[256 + i];

        i32 result_state_remove = numbers::op::ne(begin_remove, end_remove); 
        u32 result_state_remove_mask = 0 - result_state_remove;

        idx += result_state_remove;
        count_remove += result_state_remove;

        index_filter[ idx & result_state_remove_mask ] = i;
    }
    
    for(u32 i=0; i<count_insert; i++){
        const u16 idx = index_filter[i+1];
        const u16 begin_offset = market_indices_begin[idx];
        const u16 end_offset = market_indices_end[idx];
        create_insert_order_inc(data+begin_offset, market_data[i], begin_offset, end_offset);
    }

    for(u32 i=count_insert; i<num_entries; i++){
        const u16 idx = index_filter[i+1];
        const u16 begin_offset = market_indices_begin[256 + idx];
        const u16 end_offset = market_indices_end[256 + idx];
        create_remove_order_inc(data+begin_offset, market_data[i], begin_offset, end_offset);
    }

};



u32 Decoder::decode_any(const char* data){
    using namespace ctrader::tools;
    const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH_CHAR);
    const char msg_type = data[12 + msg_size_digit_size + 4];

    switch(msg_type){
        case 'X': { __DECODE_CASE(DECODE_TYPE::MARKET_DATA_INCREMENTAL) }
    }

    return 0;
}



// 8=FIX.4.4|9=1128|35=X|34=3|
// void Decoder::decode_any(const char* data){
//     using namespace ctrader::tools;
    
//     const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH_CHAR);
//     const char msg_type = data[12 + msg_size_digit_size + 4];
//     const u32 msg_seq_num_digit_size = memory::find<20>(data+12+msg_size_digit_size+9, __SETTINGS_SOH_CHAR);

//     switch(msg_type){
//         case 'X': { __DECODE_CASE(DECODE_TYPE::MARKET_DATA_INCREMENTAL) }
//     }

// }


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