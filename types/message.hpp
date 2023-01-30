#pragma once

#include <string_view>
#include <type_traits>
#include <stdint.h>

namespace ctrader::types::message {

    enum class MSG : uint8_t {
        TEST_REQ,
        LOGON, 
        MD_REQ_SUB_DEPTH, MD_REQ_SUB_SPOT, MD_REQ_UNSUB_DEPTH, MD_REQ_UNSUB_SPOT, 
    };

    #define __COMPILE_TIME_CHECK_MSG_TYPE(MSG_VALUE, MSG_TYPE_COMPARE) \
            std::is_same_v< \
                std::conditional_t<(MSG_VALUE == MSG::MSG_TYPE_COMPARE), std::true_type, std::false_type>, \
                std::true_type \
            > \

    enum class CONN : uint8_t {
        QUOTE, TRADE
    };

    constexpr char MSG_LOOKUP[] = {
        '1', 
        'A', 
        'V', 'V', 'V', 'V'
    };

    constexpr std::string_view CONN_LOOKUP[2] = { "QUOTE", "TRADE" };

} // ctrader::types::message