#pragma once

#include "entry.hpp"
#include "../../debug_settings.hpp"
#include "../../encode_settings.hpp"
#include "../../types/encode.hpp"
#include "../../tools/math.hpp"

namespace ctrader {

	template<encode_options Settings, request Request> struct body {
		using type = body<Settings, Request>;
		union {
			struct {
				entry<3U, Settings.max_test_id_digits> test_req_id; // ???={0-9;A-Z:MAX_TEST_ID_DIGITS}
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << self.entry.test_req_id;
			return os;
		}
	};

	template<encode_options Settings>
	struct body<Settings, request::logon> {
		using type = body<Settings, request::logon>;
		union {
			struct {
				entry<2U, 1U> encrypt_method; // 98={0?1}
				entry<3U, cv_digit_count(Settings.heartbeat_sec)> heartbeat; // 108={0-9:HEARTBEAT_SEC}
				entry<3U, 1U> reset_seq_num_flag; //141={Y?N}
				entry<3U, string_length(Settings.user_name)> username; // 553={USER_NAME}
				entry<3U, string_length(Settings.password)> password; // 554={PASSWORD}
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os
				<< self.entry.encrypt_method
				<< self.entry.heartbeat
				<< self.entry.reset_seq_num_flag
				<< self.entry.username
				<< self.entry.password;

			return os;
		}
	};

	template<encode_options Settings>
	struct body<Settings, request::market_data_req> {
		using type = body<Settings, request::market_data_req>;
		union {
			struct {
				entry<3U, Settings.max_req_id_digits> md_req_id; // 262={0-9:MAX_REQ_ID_DIGITS}
				entry<3U, 1U> subscription_req_type; // 263={0?1}
				entry<3U, 1U> market_depth; // 264={0?1}
				entry<3U, 1U> md_update_type; // 265={0?1}
				entry<3U, 1U> entry_type_count; // 267={0?1}
				entry<3U, 1U> entry_bid; // 269={0?1}
				entry<3U, 1U> entry_offer; // 269={0?1}
				entry<3U, 1U> symbol_count; // 146={0?1}

				// 55={SYMBOL_IS_DIGIT_ONLY ? 0-9:MAX_SYMBOL_DIGITS : 0-9;A-Z:MAX_SYMBOL_DIGITS}
				entry<2U, Settings.max_symbol_digits> symbol;
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os
				<< self.entry.md_req_id
				<< self.entry.subscription_req_type
				<< self.entry.market_depth
				<< self.entry.md_update_type
				<< self.entry.entry_type_count
				<< self.entry.entry_bid
				<< self.entry.entry_offer
				<< self.entry.symbol_count
				<< self.entry.symbol;

			return os;
		}
	};

	
	//template<> struct body<request::market_data_req> {
	//	using type = body<request::market_data_req>;
	//	union {
	//		struct {
	//			entry<, > ;
	//		} entry;
	//		char raw[sizeof(entry)];
	//	};

	//	friend std::ostream& operator<<(std::ostream& os, const type& self) {
	//		os
	//			<< self.entry. ;
	//		return os;
	//	}
	//};

}