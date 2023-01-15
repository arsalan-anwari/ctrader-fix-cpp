#pragma once

#include <cstring>
#include <string>

#include "data/message_data.hpp"
#include "data/timestamp.hpp"

#include "tools/protocol.hpp"
#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

namespace ctrader::parser::encode{

using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;
using namespace ctrader::tools;
using namespace ctrader::data;

struct Encoder{

    Encoder() :
        __msg_seq_num(1),
        __timestamp_zero_buff( std::string(
            message_data::LOGON.header.field.timestamp_32a,
            message_data::LOGON.header.field.timestamp_32a+32) 
        )
    {};

    template<MSG_TYPE T, typename... FIELD_TYPE> void modify_message(FIELD_TYPE... fields);

    uint64_t __msg_seq_num;
    std::string __timestamp_zero_buff;

};


template<> void Encoder::modify_message<MSG_TYPE::LOGON>(){
    numbers::to_string(
        message_data::LOGON.header.field.MsgSeqNum+4,
        message_data::LOGON.header.field.MsgSeqNum+18+4,
        __msg_seq_num++ 
    );

    datetime::current_timestamp_from_offset(
        message_data::LOGON.header.field.timestamp_32a,
        __timestamp_zero_buff.c_str(),
        timestamp::TimestampOffset
    );

    uint8_t cs = protocol::calc_checksum<sizeof(message_data::LOGON.data)-7>(message_data::LOGON.data);
    std::memset(message_data::LOGON.checksum+4, '0', 3);
    numbers::to_string(
        message_data::LOGON.checksum+4,
        message_data::LOGON.checksum+4+3,
        cs
    );
};

}
