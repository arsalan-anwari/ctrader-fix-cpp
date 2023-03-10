#pragma once

#include <string_view>
#include "types/numbers.hpp"
#include "tools/macros.hpp"

namespace ctrader::types::encode {
    
    using namespace ctrader::types::numbers;

    enum class ENCODE_TYPE : u8 {
        HEART_BEAT, TEST_REQ,
        LOGON, LOGOUT, RESEND_REQ,
        MD_REQ_SINGLE, MD_REQ_MULTIPLE
    };

    constexpr char ENCODE_CHAR[] = {
        '0', '1', 
        'A', '5', '2',
        REPEAT_LIST('V', 2)
    };

    enum class MD_REQ_SUB_TYPE : char {
        SUBSCRIBE = '1',
        UNSUBSCRIBE = '2'
    };

    enum class MD_REQ_DEPTH_TYPE : char {
        FULL_DEPTH = '0',
        SPOT_PRICE = '1'
    };

    enum class CONN_TYPE : u8 {
        QUOTE, TRADE
    };

    constexpr std::string_view CONN_STRING[2] = { "QUOTE", "TRADE" };

} // ctrader::types::encode