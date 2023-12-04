#pragma once

#include <string_view>

#include "types/numbers.hpp"

namespace ctrader {

	// Settings used for optimization during compilation and debugging
	namespace debug_settings {

		constexpr bool SSE_AVAILABLE = true;
		constexpr bool AVX_AVAILABLE = true;
		constexpr bool AVX2_AVAILABLE = true;
		constexpr bool AVX512_AVAILABLE = false;

		constexpr std::string_view SOH = "|"; // Use value '\1' if in production

	} // ctrader::debug_settings

	// Settings used by encoder/decoder and other helper functions
	namespace global_settings {

		// Change this if your account details require more characters.  
		constexpr u8 MAX_FIX_VERSION_SIZE = 32u;
		constexpr u8 MAX_USERNAME_SIZE = 64u;
		constexpr u8 MAX_PASSWORD_SIZE = 64u;
		constexpr u8 MAX_HEARTBEAT_SIZE = 32u;

		constexpr u8 MAX_SENDER_COMP_ID_SIZE = 64;
		constexpr u8 MAX_TARGET_COMP_ID_SIZE = 32u;
		constexpr u8 MAX_TARGET_SUB_ID_SIZE = 32u;
		
		constexpr u8 MAX_DATE_TIME_MASK_SIZE = 32u;


	} // ctrader::global_settings

} // ctrader