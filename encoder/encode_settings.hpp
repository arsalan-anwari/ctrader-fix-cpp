#pragma once

#include <string>

#include "../types/numbers.hpp"
#include "../types/datetime.hpp"
#include "../settings.hpp"

namespace ctrader {
namespace encode {

using encode_settings_t = struct {
    const char fix_version[global_settings::MAX_FIX_VERSION_SIZE];
    const char user_name[global_settings::MAX_USERNAME_SIZE];
    const char password[global_settings::MAX_PASSWORD_SIZE];

    const char heartbeat_sec[global_settings::MAX_HEARTBEAT_SIZE];

    const char sender_comp_id[global_settings::MAX_SENDER_COMP_ID_SIZE];
    const char target_comp_id[global_settings::MAX_TARGET_COMP_ID_SIZE];
    const std::array<const char[global_settings::MAX_TARGET_SUB_ID_SIZE], 2> target_sub_id;
    
    const char date_time_mask[global_settings::MAX_DATE_TIME_MASK_SIZE];
    const utc_time_offset_t date_time_mask_offsets;

    const bool symbol_is_digit_only;
    const u8 max_symbol_digits;

    const u8 max_msg_size_digits;
    const u8 max_msg_type_digits;
    const u8 max_seq_num_digits;
    const u8 max_test_id_digits;
    const u8 max_req_id_digits;

};

static constexpr auto default_encode_settings = encode_settings_t{
    .fix_version = "FIX.4.4",
    .user_name = "<<user_name>>",
    .password = "<<password>>",

    .heartbeat_sec = "30",

    .sender_comp_id = "<<sender_comp_id>>",
    .target_comp_id = "cServer",
    .target_sub_id = { "QUOTE", "TRADE" },
    
    .date_time_mask = "00000000-00:00:00.000000",
    .date_time_mask_offsets = default_utc_time_offset,
    
    .symbol_is_digit_only = true,
    .max_symbol_digits = 19u,

    .max_msg_size_digits = 3u,
    .max_msg_type_digits = 1u,

    .max_seq_num_digits = 19u,
    .max_test_id_digits = 10u,
    .max_req_id_digits = 7u
};

}} // ctrader::encode