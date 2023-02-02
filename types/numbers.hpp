#pragma once

#include <concepts>

namespace ctrader::types::numbers {

    template <typename T> requires std::integral<T>
    struct number_info_t {
        T value;
        T digit_count;
    };

} // ctrader::types::numbers