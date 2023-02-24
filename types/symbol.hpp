#pragma once

#include "types/numbers.hpp"


namespace ctrader::types::symbol {
    using namespace ctrader::types::numbers;

    enum class SYMBOL : u64 {
        UNKNOWN = 0UL,
        EUR_USD = 1UL
    };

    constexpr u8 SYMBOL_DIGIT_SIZE[] = {
        1, 1
    };

    const char* const SYMBOL_STRING[] = {
        "0", "1"
    };   

    

}