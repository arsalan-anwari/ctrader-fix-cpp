#pragma once

#include <string_view>
#include <type_traits>
#include <stdint.h>

namespace ctrader::types::encode {

    enum class ENCODE_TYPE : uint8_t {
        TEST_REQ,
        LOGON, 
        MD_REQ_SUB_DEPTH, MD_REQ_SUB_SPOT, MD_REQ_UNSUB_DEPTH, MD_REQ_UNSUB_SPOT, 
    };

    constexpr char ENCODE_CHAR[] = {
        '1', 
        'A', 
        'V', 'V', 'V', 'V'
    };

    #define __COMPILE_TIME_CHECK_TYPE(VALUE, COMPARE) \
            std::is_same_v< \
                std::conditional_t<(VALUE == COMPARE), std::true_type, std::false_type>, \
                std::true_type \
            > \

    enum class CONN_TYPE : uint8_t {
        QUOTE, TRADE
    };

    constexpr std::string_view CONN_STRING[2] = { "QUOTE", "TRADE" };

} // ctrader::types::encode