#pragma once

#include <cstring>
#include <string>

#include "data/message_data.hpp"
#include "data/timestamp.hpp"

#include "tools/protocol.hpp"
#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

namespace ctrader::parser {

using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;
using namespace ctrader::tools;
using namespace ctrader::data;

namespace internal {

    static uint64_t msg_seq_num = 0;
    static const auto timestamp_zero_buff = memory::simple_buffer_from_buffer<char, 32>( message_data::LOGON.header.field.timestamp_32a );

} // internal

template<MSG_TYPE T, typename... FIELD_TYPE> 
static inline __attribute__((always_inline))
void encode_message(FIELD_TYPE... fields);


template<> void encode_message<MSG_TYPE::LOGON>(){
    numbers::to_string(
        message_data::LOGON.header.field.MsgSeqNum+4,
        message_data::LOGON.header.field.MsgSeqNum+settings::MsgSeqNumDigitSize+4,
        internal::msg_seq_num++ 
    );

    datetime::current_timestamp_from_offset(
        message_data::LOGON.header.field.timestamp_32a,
        internal::timestamp_zero_buff.data,
        timestamp::TimestampOffset
    );

    const uint8_t cs = protocol::calc_checksum<sizeof(message_data::LOGON.data)-7>(message_data::LOGON.data);
    std::memset(message_data::LOGON.checksum+4, '0', 3);
    numbers::to_string(
        message_data::LOGON.checksum+4,
        message_data::LOGON.checksum+4+3,
        cs
    );
};

template<> void encode_message<MSG_TYPE::TEST_REQ>(){
    numbers::to_string(
        message_data::TEST_REQ.header.field.MsgSeqNum+4,
        message_data::TEST_REQ.header.field.MsgSeqNum+settings::MsgSeqNumDigitSize+4,
        internal::msg_seq_num++ 
    );

    datetime::current_timestamp_from_offset(
        message_data::TEST_REQ.header.field.timestamp_32a,
        internal::timestamp_zero_buff.data,
        timestamp::TimestampOffset
    );

    const uint8_t cs = protocol::calc_checksum<sizeof(message_data::TEST_REQ.data)-7>(message_data::TEST_REQ.data);
    std::memset(message_data::TEST_REQ.checksum+4, '0', 3);
    numbers::to_string(
        message_data::TEST_REQ.checksum+4,
        message_data::TEST_REQ.checksum+4+3,
        cs
    );
};

}
