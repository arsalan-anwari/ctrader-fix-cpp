#pragma once

#include <string_view>
#include "types/numbers.hpp"

namespace ctrader::types::encode {
    
    using namespace ctrader::types::numbers;

    enum class ENCODE_TYPE : u8 {
        TEST_REQ,
        LOGON, 
        MD_REQ_SUB_DEPTH, MD_REQ_SUB_SPOT, MD_REQ_UNSUB_DEPTH, MD_REQ_UNSUB_SPOT, 
    };

    constexpr char ENCODE_CHAR[] = {
        '1', 
        'A', 
        'V', 'V', 'V', 'V'
    };

    enum class CONN_TYPE : u8 {
        QUOTE, TRADE
    };

    constexpr std::string_view CONN_STRING[2] = { "QUOTE", "TRADE" };

} // ctrader::types::encode