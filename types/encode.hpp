#pragma once

#include <array>
#include <string_view>

#include "numbers.hpp"
#include "datetime.hpp"

#include "../encode_settings.hpp"

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

    using encode_options = struct {
        const char fix_version[encode_settings::MAX_FIX_VERSION_SIZE];
        const char user_name[encode_settings::MAX_USERNAME_SIZE];
        const char password[encode_settings::MAX_PASSWORD_SIZE];

        const char sender_comp_id[encode_settings::MAX_SENDER_COMP_ID_SIZE];
        const char target_comp_id[encode_settings::MAX_TARGET_COMP_ID_SIZE];
        const std::array<const char[encode_settings::MAX_TARGET_SUB_ID_SIZE], 2> target_sub_id;

        const char date_time_mask[encode_settings::MAX_DATE_TIME_MASK_SIZE];
        const utc_time_offset date_time_mask_offsets;

        const u8 heartbeat_sec;

        const u8 max_symbol_digits;
        const u8 max_msg_size_digits;
        const u8 max_msg_type_digits;
        const u8 max_seq_num_digits;
        const u8 max_test_id_digits;
        const u8 max_req_id_digits;
    };

    static constexpr auto default_encode_settings = encode_options {
        .fix_version = "FIX.4.4",
        .user_name = "<<user_name>>",
        .password = "<<password>>",

        .sender_comp_id = "<<sender_comp_id>>",
        .target_comp_id = "cServer",
        .target_sub_id = { "QUOTE", "TRADE" },

        .date_time_mask = "00000000-00:00:00.000000",
        .date_time_mask_offsets = default_utc_time_offset,

        .heartbeat_sec = 30u,

        .max_symbol_digits = 19u,
        .max_msg_size_digits = 3u,
        .max_msg_type_digits = 1u,
        .max_seq_num_digits = 19u,
        .max_test_id_digits = 10u,
        .max_req_id_digits = 7u
    };

}