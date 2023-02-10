#pragma once

#include <chrono>
#include <concepts>

namespace ctrader::tools::concepts {

    using namespace ctrader::types::numbers;

    template<auto SIZE>
    concept is_32byte_alignable = SIZE % 32 == 0;

    template<auto SIZE, auto MIN>
    concept is_minimum_size = SIZE >= MIN;

    template<auto SIZE, auto MAX>
    concept is_smaller_than = SIZE < MAX;

    template<auto VAL>
    concept is_power_of_2 = (VAL > 0) && !(VAL & (VAL - 1));

    template<typename T>
    concept is_time_duration = 
        std::is_same<T, std::chrono::nanoseconds>::value ||
        std::is_same<T, std::chrono::microseconds>::value ||
        std::is_same<T, std::chrono::milliseconds>::value ||
        std::is_same<T, std::chrono::seconds>::value ||
        std::is_same<T, std::chrono::minutes>::value ||
        std::is_same<T, std::chrono::hours>::value;

} //ctrader::tools::concepts