#pragma once

#include <cstring>

#include "data/message_data.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode {

using MSG = ctrader::data::message_type::MSG;
using CONN = ctrader::data::message_type::CONN;

namespace internal {
    using namespace ctrader::tools;
    using namespace ctrader::data;
    using namespace ctrader::settings;

    static uint64_t msg_seq_num = 1;

    #define __PREPARE_HEADER(MSG_DATA) \
    numbers::to_string( message_data::MSG_DATA.header.field.MsgSeqNum+4, message_data::MSG_DATA.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
    datetime::current_timestamp_from_offset( message_data::MSG_DATA.header.field.timestamp_32a ); \

    #define __PREPARE_FOOTER(MSG_DATA) \
    const uint8_t cs = calc_checksum<sizeof(message_data::MSG_DATA.data)-7>(message_data::MSG_DATA.data); \
    std::memset(message_data::MSG_DATA.checksum+4, '0', 3); \
    numbers::to_string( message_data::MSG_DATA.checksum+4, message_data::MSG_DATA.checksum+4+3, cs); \

    #define __PREPARE_DEFAULT(MSG_DATA) \
    numbers::to_string( message_data::MSG_DATA.header.field.MsgSeqNum+4, message_data::MSG_DATA.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
    datetime::current_timestamp_from_offset( message_data::MSG_DATA.header.field.timestamp_32a ); \
    const uint8_t cs = calc_checksum<sizeof(message_data::MSG_DATA.data)-7>(message_data::MSG_DATA.data); \
    std::memset(message_data::MSG_DATA.checksum+4, '0', 3); \
    numbers::to_string( message_data::MSG_DATA.checksum+4, message_data::MSG_DATA.checksum+4+3, cs); \

    template<MSG M, CONN C, typename... FIELD_TYPE> 
    static inline __attribute__((always_inline))
    void prepare_message(FIELD_TYPE... fields){
        static_assert( 
            (M == MSG::MD_REQ_SUB_DEPTH) && (C != CONN::TRADE),
            "Message type: MD_REQ_SUB_DEPTH Cannot be used with Connection type: TRADE!"  
        );
    }

    template<> void prepare_message<MSG::LOGON, CONN::QUOTE>(){ __PREPARE_DEFAULT(quote::LOGON); };
    template<> void prepare_message<MSG::LOGON, CONN::TRADE>(){ __PREPARE_DEFAULT(trade::LOGON); };

    template<> void prepare_message<MSG::TEST_REQ, CONN::QUOTE>(){ __PREPARE_DEFAULT(quote::TEST_REQ); };
    template<> void prepare_message<MSG::TEST_REQ, CONN::TRADE>(){ __PREPARE_DEFAULT(trade::TEST_REQ); };

    template<> void prepare_message<MSG::MD_REQ_SUB_DEPTH, CONN::QUOTE>(const char* mdReqId, const int symbol){ 
        __PREPARE_HEADER(quote::MD_REQ_SUB_DEPTH);
        std::memcpy(message_data::quote::MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, FieldIDDigitSize);
        std::memset(message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4, '0', SymbolIDDigitSize);
        numbers::to_string(message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4, message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4+SymbolIDDigitSize, symbol);
        __PREPARE_FOOTER(quote::MD_REQ_SUB_DEPTH);
    };

} // internal

template<CONN C>
struct Encoder {
    Encoder(){};

    template<MSG M, typename... FIELD_TYPE> 
    static inline __attribute__((always_inline))
    void encode_message(FIELD_TYPE... fields) { internal::prepare_message<M, C>(fields...); internal::msg_seq_num++; };

    static inline __attribute__((always_inline))
    void reset_seq_num() { internal::msg_seq_num = 1; };
};




// template<MSG T, typename... FIELD_TYPE> 
// static inline __attribute__((always_inline))
// void prepare_message(FIELD_TYPE... fields);

// template<> void prepare_message<MSG::LOGON>(CONN_TYPE conn){
//     __PREPARE_HEADER(LOGON);
//     __PREPARE_FOOTER(LOGON);
// };

// template<> void prepare_message<MSG::TEST_REQ>(CONN_TYPE conn){
//     __PREPARE_HEADER(TEST_REQ);
//     __PREPARE_FOOTER(TEST_REQ);
// };

// template<> void prepare_message<MSG::MD_REQ_SUB_DEPTH>(CONN_TYPE conn, const char* mdReqId, const char* symbol){
//     __PREPARE_HEADER(MD_REQ_SUB_DEPTH);

//     std::memcpy(message_data::MD_REQ_SUB_DEPTH.body.field.Symbol+4, symbol, SymbolIDDigitSize);
//     std::memcpy(message_data::MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, FieldIDDigitSize);

//     __PREPARE_FOOTER(MD_REQ_SUB_DEPTH);
// };

}
