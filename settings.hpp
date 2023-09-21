#pragma once

#include <string_view>
#include <array>

#include "types/numbers.hpp"

namespace ctrader {

	namespace settings {

		// Debug settings to select character to represent SOH. Use value '\1' if in production
		constexpr std::string_view SOH = "|";
		constexpr char SOH_CHAR = '|';

		// Changle global settings for all brokers. 
		constexpr ctrader::u8 MAX_INT32_DIGIT_SIZE = 9U;
		constexpr ctrader::u8 MAX_SEQ_NUM_DIGITS = 19U;
		constexpr ctrader::u8 MAX_TEST_ID_DIGITS = 10U;
		constexpr ctrader::u8 MAX_REQ_ID_DIGITS = 7U;
		constexpr ctrader::u8 HEARTBEAT_SEC = 30U;
		constexpr ctrader::u8 HEARTBEAT_SEC_DIGIT_SIZE = 2U;

		constexpr bool SSE_AVAILABLE = true;
		constexpr bool AVX_AVAILABLE = true;
		constexpr bool AVX2_AVAILABLE = true;
		constexpr bool AVX512_AVAILABLE = false;

		// Non changable global settings. 
		constexpr std::string_view TARGET_COMP_ID = "cServer";
		constexpr std::array<std::string_view, 2> TARGET_SUB_ID = { "QUOTE", "TRADE" };
		constexpr std::string_view DATE_TIME_MASK = "00000000-00:00:00.000000";

		// Broker specific settings.
		namespace broker {
			
			constexpr ctrader::u8 MAX_MSG_SIZE_DIGITS = 3U;
			constexpr ctrader::u8 MAX_MSG_TYPE_DIGITS = 1U;

			constexpr bool SYMBOL_IS_DIGIT_ONLY = true;
			constexpr ctrader::u8 MAX_SYMBOL_DIGITS = 19U;

			constexpr std::string_view FIX_VERSION = "FIX.4.4";

			constexpr std::string_view USER_NAME = "8536054";
			constexpr std::string_view PASSWORD = "RGZC9bBP68VqyAY";
			constexpr std::string_view SENDER_COMP_ID = "demo.icmarkets.8536054";

		} // ctrader::settings::broker



	} // ctrader::settings

} // ctrader