#pragma once

#include <algorithm> // std::copy_n

namespace ctrader {

	template<size_t N>
	struct cstring {
		constexpr cstring(const char(&str)[N]) { std::copy_n(str, N, value); }
		char value[N];
	};

}