#pragma once

#include <string_view>
#include <array>

#include "types/numbers.hpp"
#include "tools/cstring.hpp"

namespace ctrader {

	namespace settings {

		// Global settings for all brokers. 
		constexpr ctrader::u8 MAX_INT32_DIGIT_SIZE = 9U;

		constexpr ctrader::u8 MAX_SEQ_NUM_DIGITS = 19U;
		constexpr ctrader::u8 MAX_TEST_ID_DIGITS = 10U;
		constexpr ctrader::u8 MAX_REQ_ID_DIGITS = 7U;
		constexpr ctrader::u8 HEARTBEAT_SEC = 30U;

		constexpr std::string_view SOH = "|";
		constexpr char SOH_CHAR = '|';

		constexpr bool SSE_AVAILABLE = true;
		constexpr bool AVX_AVAILABLE = true;
		constexpr bool AVX2_AVAILABLE = true;
		constexpr bool AVX512_AVAILABLE = false;

		// Broker specific settings.
		namespace broker {
			
			// Private Broker specific settings used for compilation. 
			namespace {
				constexpr std::string_view DATE_TIME_MASK_START_TAG = "52=";
				constexpr std::string_view DATE_TIME_MASK_END_TAG = "49=";
			}

			constexpr ctrader::u8 MAX_MSG_SIZE_DIGITS = 3U;
			constexpr ctrader::u8 MAX_MSG_TYPE_DIGITS = 1U;

			constexpr bool SYMBOL_IS_DIGIT_ONLY = true;
			constexpr ctrader::u8 MAX_SYMBOL_DIGITS = 19U;

			constexpr std::string_view FIX_VERSION = "FIX.4.4";

			constexpr std::string_view USER_NAME = "8536054";
			constexpr std::string_view PASSWORD = "RGZC9bBP68VqyAY";
			constexpr std::string_view SENDER_COMP_ID = "demo.icmarkets.8536054";
			constexpr std::string_view TARGET_COMP_ID = "cServer";
			constexpr std::array<std::string_view, 2> TARGET_SUB_ID = { "QUOTE", "TRADE" };

			constexpr std::string_view DATE_TIME_FMT = "00000000-00:00:00.000000";
			constexpr std::string_view DATE_TIME_MASK_START = cstr_joined_v<SOH, DATE_TIME_MASK_START_TAG>;
			constexpr std::string_view DATE_TIME_MASK_END = cstr_joined_v<SOH, DATE_TIME_MASK_END_TAG>;

		} // ctrader::settings::broker

		constexpr std::string_view DATE_TIME_MASK = 
			cstr_joined_v<broker::DATE_TIME_MASK_START, broker::DATE_TIME_FMT, broker::DATE_TIME_MASK_END>;


	} // ctrader::settings

} // ctrader