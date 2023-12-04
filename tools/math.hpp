#pragma once

#include <string>
#include <cmath> // std::log10, std::floor, std::abs
#include <concepts> // std::integral
#include <type_traits> // std::is_constant_evaluated()

#include "../types/numbers.hpp"

namespace ctrader {

    inline constexpr u8 string_length(const char data[]) {
		return static_cast<u8>(std::string(data).size());
	}

    template<typename T> requires std::integral<T>
	inline constexpr T digit_count(T value) {		
		return static_cast<T>(std::floor(std::log10(value)) + 1U);
	}

	template<typename T> requires std::integral<T>
	consteval T cv_digit_count(T value) {
		T counter = 0;
		while (value != 0) {
			value /= 10;
			counter++;
		}
		return counter;
	}

	inline constexpr i32 ceil(float num) {
		i32 inum = static_cast<i32>(num);
		if (num == static_cast<float>(inum)) {
			return inum;
		}
		return inum + 1;
	}

}

