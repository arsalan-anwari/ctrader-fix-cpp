#pragma once

#include "numbers.hpp"

namespace ctrader {

	enum class symbol : u64 {
		undefined = 0,
		eur_usd = 1
	};

	constexpr u8 SYMBOL_DIGIT_SIZE[] = {
		1U, 
		1U
	};


}