#pragma once

#include <stdint.h>
// #include <string_view>

namespace ctrader::types::symbol {

    enum class SYMBOL : uint64_t {
        UNKNOWN = 0UL,
        EUR_USD = 1UL
    };

    constexpr SYMBOL SYMBOL_LOOKUP[] = {
        SYMBOL::UNKNOWN, SYMBOL::EUR_USD
    };

    constexpr uint8_t SYMBOL_DIGIT_SIZE[] = {
        1, 1
    };

    const char* const SYMBOL_STRING[] = {
        "0", "1"
    };   

    

}