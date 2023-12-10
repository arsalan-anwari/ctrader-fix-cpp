#pragma once

#include <string_view>

#include "types/numbers.hpp"

// Settings used for optimization during compilation and debugging
namespace ctrader {
namespace debug_settings {

constexpr bool SSE_AVAILABLE = true;
constexpr bool AVX_AVAILABLE = true;
constexpr bool AVX2_AVAILABLE = true;
constexpr bool AVX512_AVAILABLE = false;

constexpr std::string_view SOH = "|"; // Use value '\1' if in production

}} // ctrader::debug_settings