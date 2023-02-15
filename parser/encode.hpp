#pragma once

#include <cstring>
#include <utility>

#include "data/message_data.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode {

using namespace ctrader::types::encode;
using namespace ctrader::tools;

namespace {
    using namespace ctrader::types::symbol;
    using namespace ctrader::data;
    using namespace ctrader::settings;

    // constexpr i64 const base_lookup[] = { 
    //         1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 
    //         1'000'000'000, 10'000'000'000, 100'000'000'000, 1'000'000'000'000,
    //         10'000'000'000'000, 100'000'000'000'000, 1'000'000'000'000'000, 
    //         10'000'000'000'000'000, 100'000'000'000'000'000, 1'000'000'000'000'000'000
    // };

    #define __PREPARE_HEADER(MSG_DATA) \
    numbers::to_string( message_data::MSG_DATA.header.field.MsgSeqNum+4, message_data::MSG_DATA.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
    datetime::current_timestamp_from_offset( message_data::MSG_DATA.header.field.timestamp_32a ); \


    #define __PREPARE_FOOTER(MSG_DATA) \
    const u8 cs = calc_checksum<sizeof(message_data::MSG_DATA.data)-7>(message_data::MSG_DATA.data); \
    std::memset(message_data::MSG_DATA.checksum+4, '0', 3); \
    numbers::to_string( message_data::MSG_DATA.checksum+4, message_data::MSG_DATA.checksum+4+3, cs); \


    #define __PREPARE_DEFAULT(MSG_DATA) \
    numbers::to_string( message_data::MSG_DATA.header.field.MsgSeqNum+4, message_data::MSG_DATA.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
    datetime::current_timestamp_from_offset( message_data::MSG_DATA.header.field.timestamp_32a ); \
    const u8 cs = calc_checksum<sizeof(message_data::MSG_DATA.data)-7>(message_data::MSG_DATA.data); \
    std::memset(message_data::MSG_DATA.checksum+4, '0', 3); \
    numbers::to_string( message_data::MSG_DATA.checksum+4, message_data::MSG_DATA.checksum+4+3, cs); \


    template<ENCODE_TYPE M, CONN_TYPE C, typename... FIELD_TYPE> 
    static inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void prepare_message(const i64 msg_seq_num, FIELD_TYPE... fields){
        static_assert( 
            (M == ENCODE_TYPE::MD_REQ_SUB_DEPTH) && (C != CONN_TYPE::TRADE),
            "Message type: MD_REQ_SUB_DEPTH Cannot be used with Connection type: TRADE!"  
        );
    }

    template<> void prepare_message<ENCODE_TYPE::LOGON, CONN_TYPE::QUOTE>(const i64 msg_seq_num){ __PREPARE_DEFAULT(quote::LOGON); };
    template<> void prepare_message<ENCODE_TYPE::LOGON, CONN_TYPE::TRADE>(const i64 msg_seq_num){ __PREPARE_DEFAULT(trade::LOGON); };

    template<> void prepare_message<ENCODE_TYPE::TEST_REQ, CONN_TYPE::QUOTE>(const i64 msg_seq_num){ __PREPARE_DEFAULT(quote::TEST_REQ); };
    template<> void prepare_message<ENCODE_TYPE::TEST_REQ, CONN_TYPE::TRADE>(const i64 msg_seq_num){ __PREPARE_DEFAULT(trade::TEST_REQ); };

    template<> void prepare_message<ENCODE_TYPE::MD_REQ_SUB_DEPTH, CONN_TYPE::QUOTE>(const i64 msg_seq_num, const SYMBOL symbol, const char* mdReqId ){ 
        __PREPARE_HEADER(quote::MD_REQ_SUB_DEPTH);
        
        std::memcpy(message_data::quote::MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, KeySize);
        
        std::memset(message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4, '0', 20);

        u64 symbol_id = static_cast<u64>(symbol);
        
        std::memcpy(
            message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4+(20-SYMBOL_DIGIT_SIZE[symbol_id]), 
            SYMBOL_STRING[symbol_id], 
            SYMBOL_DIGIT_SIZE[symbol_id]
        );

        __PREPARE_FOOTER(quote::MD_REQ_SUB_DEPTH);
    };

} // internal

template <CONN_TYPE C>
struct Encoder {
   
    template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    inline __attribute__((always_inline))
    void encode_message(FIELD_TYPE... fields) { 
        prepare_message<M, C>( msg_seq_num, fields... ); 
        advance_seq_num();
    };

    inline __attribute__((always_inline))
    void advance_seq_num() { 
        msg_seq_num++;
        
        i32 res = msg_seq_num - msg_seq_num_base;
        i32 is_overflow = numbers::op::gte(res, -1);
        u32 is_overflow_mask = 0 - is_overflow;

        msg_seq_num_base = (msg_seq_num_base << (3 & is_overflow_mask) ) + ( (msg_seq_num_base << 1) & is_overflow_mask );
        msg_seq_num_digit_size += is_overflow;
    };

    inline __attribute__((always_inline))
    void reduce_seq_num() { 
        msg_seq_num--;
        
        i32 res = msg_seq_num - base_lookup[msg_seq_num_digit_size - 1];
        i32 is_underflow = numbers::op::lte(res, -1);

        msg_seq_num_digit_size -= is_underflow;
        msg_seq_num_base = base_lookup[msg_seq_num_digit_size];
    };

    inline __attribute__((always_inline))
    void reset_seq_num() { msg_seq_num = 1; msg_seq_num_base = 10; msg_seq_num_digit_size = 1; };

    inline __attribute__((always_inline))
    i64 get_seq_num() { return msg_seq_num; };

    inline __attribute__((always_inline))
    i64 get_seq_num_base() { return msg_seq_num_base; };

    inline __attribute__((always_inline))
    u32 get_seq_num_digit_size() { return msg_seq_num_digit_size; };

private:
        i64 msg_seq_num = 1;
        i64 msg_seq_num_base = 10;
        u32 msg_seq_num_digit_size = 1;

        constexpr static i64 base_lookup[18] = { 
            1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 
            1'000'000'000, 10'000'000'000, 100'000'000'000, 1'000'000'000'000,
            10'000'000'000'000, 100'000'000'000'000, 1'000'000'000'000'000, 
            10'000'000'000'000'000, 100'000'000'000'000'000, 1'000'000'000'000'000'000
        };

};



}
