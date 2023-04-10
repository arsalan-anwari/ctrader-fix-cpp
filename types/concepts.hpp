#pragma once

#include <chrono>
#include <concepts>
#include "exec_policy.hpp"

namespace ctrader{
namespace concepts {

    template<auto SIZE>
    concept is_32byte_alignable = SIZE % 32 == 0;

    template<auto SIZE, auto MIN>
    concept is_min_size = SIZE >= MIN;

    template<auto SIZE, auto MAX>
    concept is_max_size = SIZE <= MAX;

    template<auto SIZE, auto MIN, auto MAX>
    concept is_within_range = SIZE >= MIN && SIZE <= MAX;

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

    template<size_t Stride, exec_policy Policy>
    concept is_vectorizable = is_power_of_2<Stride> && Policy != exec_policy::scalar;

}
} 

	

