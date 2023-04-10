#pragma once

#include <cmath> // std::log10, std::floor, std::abs
#include <concepts>

#include "../types/numbers.hpp"

namespace ctrader {

    template<typename T> requires std::integral<T>
    inline constexpr T digit_count(T value) {
        return (std::floor(std::log10(value))) + 1;
    }

	consteval i32 ceil(float num) {
		i32 inum = static_cast<i32>(num);
		if (num == static_cast<float>(inum)) {
			return inum;
		}
		return inum + 1;
	}

}

