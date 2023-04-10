#pragma once

#include <string_view>

#include "numbers.hpp"

namespace ctrader {

	enum class connection: u8 {
		trade, quote
	};

	constexpr std::string_view CONNECTION_NAME[] = {
		"TRADE", "QUOTE"
	};

	enum class subscription : char {
		enable = '1', disable = '2'
	};

	enum class market_depth : char {
		full = '0', spot = '1'
	};

}