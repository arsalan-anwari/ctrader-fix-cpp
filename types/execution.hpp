#pragma once

#include "numbers.hpp"

namespace ctrader::types::execution {

    using namespace ctrader::types::numbers;

    enum class EXEC_TYPE : u8 {
        SCALAR, AVX, AVX2, AVX512
    };

    enum class BUFF_TYPE : u8 {
        MIN, SMALL, MEDIUM, LARGE, MAX
    };

}