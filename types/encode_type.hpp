#pragma once

#include <string>
#include <stdint.h>

#include "settings.hpp"
#include "message_type.hpp"

namespace ctrader::types::encode_type {

using namespace ctrader::types::message_type;

namespace internal{
    using namespace ctrader::settings;
    constexpr std::string dummyFieldID = std::string(KeySize, '0');
}

template<MSG T> struct options_t {};

template<> struct options_t<MSG::LOGON> {};
template<> struct options_t<MSG::TEST_REQ> {};

template<> struct options_t<MSG::MD_REQ_SUB_DEPTH> { 
    const char* mdReqId = internal::dummyFieldID.c_str(); 
    int64_t symbol = 0; 
};

}