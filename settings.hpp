#pragma once

#include <string_view>

#include "types/numbers.hpp"

namespace ctrader {

	namespace settings {

		constexpr ctrader::u8 MAX_SEQ_NUM_DIGITS = 19;
		constexpr ctrader::u8 MAX_REQ_ID_DIGITS = 7;

		constexpr std::string_view SOH = "|";
		constexpr char SOH_CHAR = '|';
		constexpr std::string_view HEARTBEAT_SEC = "30";
		constexpr std::string_view DATE_TIME_MASK = "|52=00000000-00:00:00.000000|49=";

		constexpr bool SSE_AVAILABLE = false;
		constexpr bool AVX_AVAILABLE = false;
		constexpr bool AVX2_AVAILABLE = false;
		constexpr bool AVX512_AVAILABLE = false;

		namespace broker {
			constexpr std::string_view USER_NAME = "8536054";
			constexpr std::string_view PASSWORD = "RGZC9bBP68VqyAY";
			constexpr std::string_view SENDER_COMP_ID = "demo.icmarkets.8536054";

		} // ctrader::settings::broker


	} // ctrader::settings

} // ctrader