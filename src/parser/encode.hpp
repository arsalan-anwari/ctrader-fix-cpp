#pragma once

#include <cstring>

#include "data/message_data.hpp"
#include "data/message_metadata.hpp"
#include "tools/protocol.hpp"

namespace ctrader::parser::encode{

using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;
using namespace ctrader::tools::protocol;
using namespace ctrader::data;

class Encoder{
public:
    Encoder(){};

    // static uint64_t msgSeqNum = 1;
    template<MSG_TYPE T, typename... FIELD_TYPE> void modify_message(FIELD_TYPE... fields);

};


template<> void Encoder::modify_message<MSG_TYPE::LOGON>(){
    std::memset( 
        message_data::LOGON.header.raw,
        'A', 10
    );
};

}
