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

    // inline void create_insert_order_inc(const char* chunk, decode_data<DATA_TYPE::MARKET_DATA>& entry, u32 size){
    //     using namespace ctrader::tools;
    //     // example entry: |279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000

    //     entry.UpdateAction = UPDATE_ACTION::NEW;
    //     entry.EntryType = static_cast<ENTRY_TYPE>( chunk[11] - '0' );
        
    //     u32 entryLength;
    //     u32 offset = 17;

    //     entryLength = memory::find<12>(chunk+offset, __SETTINGS_SOH_CHAR);
    //     entry.EntryId = numbers::to_num<i64>(chunk+offset, entryLength);
    //     offset += entryLength + 4; // |55=

    //     entryLength = memory::find<10>(chunk+offset, __SETTINGS_SOH_CHAR);
    //     entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+offset, entryLength));
    //     offset += entryLength + 5; // |270= 

    //     entryLength = memory::find<16>(chunk+offset, __SETTINGS_SOH_CHAR);
    //     entry.EntryPrice.from_cstr(chunk+offset, entryLength);
    //     offset += entryLength + 5; // // |271=

    //     entryLength = size - offset;
    //     entry.EntrySize = numbers::to_num<i64>(chunk+offset, entryLength);
    // }

    // inline void create_remove_order_inc(const char* chunk, decode_data<DATA_TYPE::MARKET_DATA>& entry, u32 size){
    //     using namespace ctrader::tools;
    //     // example entry: |279=2|278=2291666392|55=1

    //     entry.UpdateAction = UPDATE_ACTION::DELETE;
    //     entry.EntryType = ENTRY_TYPE::UNKNOWN;
        
    //     u16 entryLength;
    //     u16 offset = 11;

    //     entryLength = memory::find<12>(chunk+offset, __SETTINGS_SOH_CHAR);
    //     entry.EntryId = numbers::to_num<i64>(chunk+offset, entryLength);
    //     offset += entryLength + 4; // |55=

    //     entryLength = size - offset;
    //     entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+offset, entryLength));
    // }

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

    inline void create_insert_order_inc(const char* chunk, u16 entryIdx, u16 size);

    inline void create_remove_order_inc(const char* chunk, u16 entryIdx, u16 size);

    template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
    inline void decode_algorithm(const char* data, const u32 data_size, const u32 num_entries);

public:
    u8 index_filter[32];
    u16 market_indices_begin[ 32 * 2 ] = { 0 };
    u16 market_indices_end[ 32 * 2 ] = { 0 };
    decode_data<DATA_TYPE::MARKET_DATA> market_data[32];

};

template<> void Decoder::decode_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>(const char* data, const u32 data_size, const u32 num_entries){
    using namespace ctrader::tools;
  
    // Setup variables
    static constexpr char pattern[32] = __DECODE_GEN_PATTERN(279);
    u32 type, mask_1, mask_2, mask_3, offset_1, offset_2, offset_3, i;
    i32 state_1, idx_seek;
    
    // Precalculate first entry (always starts at i=0)
    type = ( data[5] - '0' );
    mask_1 = 0 - op::eq(type, 2); 
    market_indices_begin[ (32 & mask_1) + 1 ] = 0;

    offset_1 = 50 - (25 & mask_1);
    mask_3 = mask_1;
    offset_3 = 2;

    // Vectoried search operation
    while( numbers::op::lte( (offset_1 + 32U), data_size) ){    
        // Check if pattern is found
        idx_seek = memory::find_pattern_begin(data+offset_1, pattern);
        state_1 = op::gte(idx_seek, 0);
        mask_1 = 0 - state_1;

        // If found: Get message type Else: Set type to default.
        offset_2 = (offset_1 + idx_seek);
        type = ( data[ (offset_2 & mask_1) + 5 ] - '0' );
        mask_2 = 0 - op::eq(type, 2); 

        // If found: Insert in market_indices_begin/end[idx_begin/end] Else: Ignore (insert in market_indices_begin/end[0]) 
        market_indices_begin[ ((32 & mask_2) + offset_3) & mask_1 ] = offset_2;
        market_indices_end[ ((32 & mask_3) + (offset_3 - 1)) & mask_1 ] = offset_2; 
        mask_3 = mask_2;
        offset_3 += state_1;

        // Calc skip size
        offset_1 += ((4 & ~mask_1) + ((25 + (25 & ~mask_2) + idx_seek) & mask_1));  
    }

    // Handle last part of market_indices
    idx_seek = memory::find_pattern_begin(data+offset_1, pattern);
    offset_2 = offset_1 + idx_seek;

    type = ( data[ offset_2 + 5 ] - '0' );
    mask_1 = 0 - op::eq(type, 2); 

    market_indices_begin[ ((32 & mask_1) + offset_3) ] = offset_2;
    market_indices_end[ ((32 & mask_3) + (offset_3 - 1)) ] = offset_2; 
    market_indices_end[ ((32 & mask_1) + offset_3) ] = data_size;

    // Filter out incorrect entries to index_filter
    offset_1 = offset_2 = 0;
    for(i=1; i<num_entries+1; i++){
        state_1 = numbers::op::ne( market_indices_begin[i], market_indices_end[i] );
        mask_1 = 0 - state_1; offset_1 += state_1; 
        index_filter[ offset_1 & mask_1 ] = i;
    }

    for(i=1; i<num_entries+1; i++){
        state_1 = numbers::op::ne( market_indices_begin[32+i], market_indices_end[32+i] );
        mask_1 = 0 - state_1; offset_2 += state_1; 
        index_filter[ (offset_1 + offset_2) & mask_1 ] = i;
    }

    // Create market data from filtered entries index_filter 
    for(i=1; i<offset_1+1; i++){
        idx_seek = index_filter[i];
        mask_1 = market_indices_begin[idx_seek];
        mask_2 = market_indices_end[idx_seek];
        create_insert_order_inc(data+mask_1, i-1, (mask_2 - mask_1));
    }

    for(i=1; i<offset_2+1; i++){
        mask_3 = offset_1 + i;
        idx_seek = 32 + index_filter[mask_3];
        mask_1 = market_indices_begin[idx_seek];
        mask_2 = market_indices_end[idx_seek];
        create_remove_order_inc(data+mask_1, mask_3-1, (mask_2 - mask_1));
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

void Decoder::create_insert_order_inc(const char* chunk, u16 entryIdx, u16 size){
    using namespace ctrader::tools;
    // example entry: |279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000
    auto& entry = market_data[entryIdx];

    entry.UpdateAction = UPDATE_ACTION::NEW;
    entry.EntryType = static_cast<ENTRY_TYPE>( chunk[11] - '0' );
    
    u32 entryLengthMask, offset; 
    i32 entryLength1, entryLength2, entryLength3;

    entryLengthMask = memory::find_mask<32>(chunk+17, __SETTINGS_SOH_CHAR);

    entryLength1 = __builtin_ctz(entryLengthMask);
    entryLengthMask >>= entryLength1 + 4;

    entryLength2 = __builtin_ctz(entryLengthMask);
    entryLengthMask >>= entryLength2 + 5;

    entryLength3 = __builtin_ctz(entryLengthMask);

    offset = 17+entryLength1+4+entryLength2+5;

    entry.EntryPrice.from_cstr(chunk+offset, entryLength3);
    entry.EntryId = numbers::to_num<i64>(chunk+17, entryLength1);
    entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+17+entryLength1+4, entryLength2));

    offset += entryLength3+5; // |271= ... |
    entry.EntrySize = numbers::to_num<i64>(chunk+offset, size - offset);
};

void Decoder::create_remove_order_inc(const char* chunk, u16 entryIdx, u16 size){
    using namespace ctrader::tools;
    // example entry: |279=2|278=2291666392|55=1
    auto& entry = market_data[entryIdx];

    entry.UpdateAction = UPDATE_ACTION::DELETE;
    entry.EntryType = ENTRY_TYPE::UNKNOWN;
    
    u16 entryLength;
    u16 offset = 11;

    entryLength = memory::find<12>(chunk+offset, __SETTINGS_SOH_CHAR);
    entry.EntryId = numbers::to_num<i64>(chunk+offset, entryLength);
    offset += entryLength + 4; // |55=

    entryLength = size - offset;
    entry.Symbol = static_cast<SYMBOL>(numbers::to_num<u64>(chunk+offset, entryLength));
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