#pragma once

#include <string>

#include "types/numbers.hpp"

namespace ctrader {
namespace encode_settings {

// Change this if your account details require more characters.  
constexpr u8 MAX_FIX_VERSION_SIZE = 32u;
constexpr u8 MAX_USERNAME_SIZE = 64u;
constexpr u8 MAX_PASSWORD_SIZE = 64u;
constexpr u8 MAX_HEARTBEAT_SIZE = 32u;

constexpr u8 MAX_SENDER_COMP_ID_SIZE = 64;
constexpr u8 MAX_TARGET_COMP_ID_SIZE = 32u;
constexpr u8 MAX_TARGET_SUB_ID_SIZE = 32u;

constexpr u8 MAX_DATE_TIME_MASK_SIZE = 32u;


}} // ctrader::encode_settings