#pragma once

#include <cstring>
#include <chrono>

#include "data/message_data.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode {

using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;

using namespace ctrader::tools;
using namespace ctrader::data;
using namespace ctrader::settings;

namespace internal {

    static uint64_t msg_seq_num = 1;
    static const char* const CONN_LOOKUP[2] { "QUOTE", "TRADE" };

#define __PREPARE_HEADER(MT) \
    numbers::to_string( message_data::MT.header.field.MsgSeqNum+4, message_data::MT.header.field.MsgSeqNum+settings::MsgSeqNumDigitSize+4, internal::msg_seq_num++ ); \
    datetime::current_timestamp_from_offset( message_data::MT.header.field.timestamp_32a ); \
    std::memcpy( message_data::MT.header.field.TargetSubID+4, internal::CONN_LOOKUP[ static_cast<uint8_t>(conn) ], 5 ); \

#define __PREPARE_FOOTER(MT) \
    const uint8_t cs = calc_checksum<sizeof(message_data::LOGON.data)-7>(message_data::LOGON.data); \
    std::memset(message_data::LOGON.checksum+4, '0', 3); \
    numbers::to_string( message_data::LOGON.checksum+4, message_data::LOGON.checksum+4+3, cs); \

} // internal

enum class CONN_TYPE : uint8_t {
    QUOTE, TRADE
};

template<MSG_TYPE T, typename... FIELD_TYPE> 
static inline __attribute__((always_inline))
void prepare_message(FIELD_TYPE... fields);

template<> void prepare_message<MSG_TYPE::LOGON>(CONN_TYPE conn){
    __PREPARE_HEADER(LOGON);
    __PREPARE_FOOTER(LOGON);
};

template<> void prepare_message<MSG_TYPE::TEST_REQ>(CONN_TYPE conn){
    __PREPARE_HEADER(TEST_REQ);
    __PREPARE_FOOTER(TEST_REQ);
};

template<> void prepare_message<MSG_TYPE::MD_REQ_SUB_DEPTH>(CONN_TYPE conn, const char* mdReqId, const char* symbol){
    __PREPARE_HEADER(MD_REQ_SUB_DEPTH);

    std::memcpy(message_data::MD_REQ_SUB_DEPTH.body.field.Symbol+4, symbol, SymbolIDDigitSize);
    std::memcpy(message_data::MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, FieldIDDigitSize);

    __PREPARE_FOOTER(MD_REQ_SUB_DEPTH);
};

}
