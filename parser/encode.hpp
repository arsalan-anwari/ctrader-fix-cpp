#pragma once

#include <cstring>
#include <utility>

#include "data/message_data.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "global.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode {

using namespace ctrader::types::symbol;
using namespace ctrader::types::message;
using namespace ctrader::parser::global;
using namespace ctrader::tools;

namespace internal {
    using namespace ctrader::data;
    using namespace ctrader::settings;

    constexpr std::string dummyFieldID = std::string(KeySize, '0');

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
     inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
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

    template<> void prepare_message<MSG::MD_REQ_SUB_DEPTH, CONN::QUOTE>(const SYMBOL symbol, const char* mdReqId ){ 
        __PREPARE_HEADER(quote::MD_REQ_SUB_DEPTH);
        
        std::memcpy(message_data::quote::MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, KeySize);
        
        std::memset(message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4, '0', 20);

        std::memcpy(
            message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4+(20-SymbolData::get_size(symbol)), 
            SymbolData::get_data(symbol), 
            SymbolData::get_size(symbol)
        );

        //numbers::to_string(message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4, message_data::quote::MD_REQ_SUB_DEPTH.body.field.Symbol+4+20, 1 );
        __PREPARE_FOOTER(quote::MD_REQ_SUB_DEPTH);
    };

} // internal

template<CONN C>
struct Encoder {
    Encoder(){};

    template<MSG M, typename... FIELD_TYPE> 
     inline __attribute__((always_inline))
    void encode_message(FIELD_TYPE... fields) { 
        internal::prepare_message<M, C>( fields... ); 
        msg_seq_num++;
        numbers::overflow_correction(
            msg_seq_num, 
            msg_seq_num_base, 
            msg_seq_num_digit_size
        );

    };


     inline __attribute__((always_inline))
    void reset_seq_num() { global::parser::msg_seq_num = 1; };
};

}
