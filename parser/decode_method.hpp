#pragma once

#include <stdio.h>

#include "types/decode.hpp"
#include "types/symbol.hpp"
#include "tools/numbers.hpp"
#include "tools/memory.hpp"

namespace ctrader::parser::decode::methods {

    using namespace ctrader::types::decode;
    using namespace ctrader::types::symbol;

namespace {
    using namespace ctrader::settings;

    #define __DECODE_GEN_PATTERN(pattern) __SETTINGS_SOH_STR #pattern

    #define __PREPARE_DECODE() \
        using namespace ctrader::tools;\
        const u32 msg_seq_num_digit_size = memory::find<20>(data+12+msg_size_digit_size+9, __SETTINGS_SOH);\
        const u32 header_size = get_message_header_size(msg_seq_num_digit_size);\
        const u32 data_size = numbers::to_num(data+12, msg_size_digit_size) - header_size - 7;\
        const u32 offset = ( 12U + msg_size_digit_size + header_size );\
        const u32 num_entries = numbers::to_num(data + offset + 5, memory::find<4>(data + offset + 5, __SETTINGS_SOH));\

    static inline u32 get_message_header_size(const u32 msg_seq_num_digit_size){
        return ( 
                9U + // |35=X|34=
                msg_seq_num_digit_size + // {0:1-18}
                49U + // |49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=
                broker_settings::SenderCompID.size() // demo.icmarkets.8536054
        );
    }
}

    template<u16 BUFF_SIZE, DATA_TYPE T> 
    struct data_decoder {};

    template<u16 BUFF_SIZE>
    struct data_decoder<BUFF_SIZE, DATA_TYPE::MARKET_DATA> {
        DATA_BUFF<DATA_TYPE::MARKET_DATA, BUFF_SIZE> market_data;
        
        u16 prepare_and_decode_incremental( const char* data, const u32 msg_size_digit_size ){            
            __PREPARE_DECODE();
            decode_incremental(data + offset + 7, data_size, num_entries);
            return num_entries;
        }

    private:
        u16 index_filter[ BUFF_SIZE ] = { 0 };
        u32 market_indices_begin[ BUFF_SIZE * 2 ] = { 0 };
        u32 market_indices_end[ BUFF_SIZE * 2 ] = { 0 };

        inline void create_inc_update_new( const char* chunk, u16 entryIdx, u16 size ){
            using namespace ctrader::tools;
            // example entry: |279=0|269=1|278=0000002291667248|55=00000000000000000001|270=0000000001.08754|271=5000000
            auto& entry = market_data[entryIdx];

            entry.UpdateAction = UPDATE_ACTION::NEW;
            entry.EntryType = static_cast<ENTRY_TYPE>( chunk[11] - '0' );
            
        #if __SETTINGS_ALLOW_RESTRICTIONS == __SETTINGS_ENABLE
            u32 entryLengthMask, offset; 
            i32 entryLength1, entryLength2, entryLength3;

            entryLengthMask = memory::find_mask<32>(chunk+17, __SETTINGS_SOH);

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
        #else
            u8 offset1, offset2, offset3;
            u8 entryLength1, entryLength2, entryLength3; 

            entryLength1 = __builtin_ctz(memory::find_mask<16>(chunk+17, __SETTINGS_SOH)); // |278= seek(...) |
            offset1 = 17 + (entryLength1 + 4);

            entryLength2 = __builtin_ctz(memory::find_mask<20>(chunk+offset1, __SETTINGS_SOH)); // |55= seek(...) |
            offset2 = offset1 + (entryLength2 + 5);

            entryLength3 = __builtin_ctz(memory::find_mask<16>(chunk+offset2, __SETTINGS_SOH)); // |270= seek(...) |
            offset3 = offset2 + (entryLength3 + 5);

            entry.EntryId = numbers::to_num<i64>(chunk+17, entryLength1 );
            entry.Symbol = static_cast<SYMBOL>( numbers::to_num<u64>( chunk+offset1, entryLength2 ) );
            entry.EntryPrice.from_cstr(chunk+offset2, entryLength3);
            entry.EntrySize = numbers::to_num<i64>(chunk+offset3, size - offset3);
        #endif

        }

        inline void create_inc_update_delete( const char* chunk, u16 entryIdx, u16 size ){
            using namespace ctrader::tools;
            // example entry: |279=2|278=2291666392|55=1
            auto& entry = market_data[entryIdx];

            entry.UpdateAction = UPDATE_ACTION::DELETE;
            entry.EntryType = ENTRY_TYPE::UNKNOWN;
            
            u32 entryLength;
            u8 offset;

            entryLength = memory::find<12>(chunk+11, __SETTINGS_SOH); // |278= seek(...) |
            entry.EntryId = numbers::to_num<i64>(chunk+11, entryLength);
            offset = 11 + entryLength + 4; // |55=

            entry.Symbol = static_cast<SYMBOL>( numbers::to_num<u64>(chunk+offset, (size - offset)) );
        }

        __attribute__((optimize("unroll-loops")))
        void decode_incremental(const char* data, const u32 data_size, const u16 num_entries){
            using namespace ctrader::tools;
  
            // Setup variables
            static constexpr char pattern[32] = __DECODE_GEN_PATTERN(279);
            u32 type, mask_1, mask_2, mask_3, offset_1, offset_2, offset_3;
            i32 state_1, idx_seek;
            u16 i;
            
            // Precalculate first entry (always starts at i=0)
            type = ( data[5] - '0' );
            mask_1 = 0 - op::eq(type, 2); 
            market_indices_begin[ (BUFF_SIZE & mask_1) + 1 ] = 0;

            offset_1 = 50 - (25 & mask_1);
            mask_3 = mask_1;
            offset_3 = 2;

            // Vectoried search operation
            while( numbers::op::lte( (offset_1 + BUFF_SIZE), data_size) ){    
                // Check if pattern is found
                idx_seek = memory::find_pattern_begin(data+offset_1, pattern);
                state_1 = op::gte(idx_seek, 0);
                mask_1 = 0 - state_1;

                // If found: Get message type Else: Set type to default.
                offset_2 = (offset_1 + idx_seek);
                type = ( data[ (offset_2 & mask_1) + 5 ] - '0' );
                mask_2 = 0 - op::eq(type, 2); 

                // If found: Insert in market_indices_begin/end[idx_begin/end] Else: Ignore (insert in market_indices_begin/end[0]) 
                market_indices_begin[ ((BUFF_SIZE & mask_2) + offset_3) & mask_1 ] = offset_2;
                market_indices_end[ ((BUFF_SIZE & mask_3) + (offset_3 - 1)) & mask_1 ] = offset_2; 
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

            market_indices_begin[ ((BUFF_SIZE & mask_1) + offset_3) ] = offset_2;
            market_indices_end[ ((BUFF_SIZE & mask_3) + (offset_3 - 1)) ] = offset_2; 
            market_indices_end[ ((BUFF_SIZE & mask_1) + offset_3) ] = data_size;

            // Filter out incorrect entries to index_filter
            offset_1 = offset_2 = 0;
            for(i=1; i<num_entries+1; i++){
                state_1 = numbers::op::ne( market_indices_begin[i], market_indices_end[i] );
                mask_1 = 0 - state_1; offset_1 += state_1; 
                index_filter[ offset_1 & mask_1 ] = i;
            }

            for(i=1; i<num_entries+1; i++){
                state_1 = numbers::op::ne( market_indices_begin[BUFF_SIZE+i], market_indices_end[BUFF_SIZE+i] );
                mask_1 = 0 - state_1; offset_2 += state_1; 
                index_filter[ (offset_1 + offset_2) & mask_1 ] = i;
            }

            // Create market data from filtered entries index_filter 
            for(i=1; i<offset_1+1; i++){
                idx_seek = index_filter[i];
                mask_1 = market_indices_begin[idx_seek];
                mask_2 = market_indices_end[idx_seek];
                create_inc_update_new(data+mask_1, i-1, (mask_2 - mask_1));
            }

            for(i=1; i<offset_2+1; i++){
                mask_3 = offset_1 + i;
                idx_seek = BUFF_SIZE + index_filter[mask_3];
                mask_1 = market_indices_begin[idx_seek];
                mask_2 = market_indices_end[idx_seek];
                create_inc_update_delete(data+mask_1, mask_3-1, (mask_2 - mask_1));
            }
        }

    };

}