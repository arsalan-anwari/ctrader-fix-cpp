#pragma once

#include "entry.hpp"
#include "../../tools/numbers.hpp"
#include "../../settings.hpp"

namespace ctrader {

	template<request T> struct body_t {
		using type = body_t<T>;
		union {
			struct {
				entry_t<3, settings::MAX_TEST_ID_DIGITS> test_req_id; // ???={0-9;A-Z:MAX_TEST_ID_DIGITS}
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << self.entry.test_req_id;
			return os;
		}
	};

	template<> struct body_t<request::logon> {
		using type = body_t<request::logon>;
		union {
			struct {
				entry_t<2, 1> encrypt_method; // 98={0?1}
				entry_t<3, settings::HEARTBEAT_SEC> heartbeat; // 108={0-9:HEARTBEAT_SEC}
				entry_t<3, 1> reset_seq_num_flag; //141={Y?N}
				entry_t<3, settings::broker::USER_NAME.size()> username; // 553={USER_NAME}
				entry_t<3, settings::broker::PASSWORD.size()> password; // 554={PASSWORD}
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

	template<> struct body_t<request::market_data_req> {
		using type = body_t<request::market_data_req>;
		union {
			struct {
				entry_t<3, settings::MAX_REQ_ID_DIGITS> md_req_id; // 262={0-9:MAX_REQ_ID_DIGITS}
				entry_t<3, 1> subscription_req_type; // 263={0?1}
				entry_t<3, 1> market_depth; // 264={0?1}
				entry_t<3, 1> md_update_type; // 265={0?1}
				entry_t<3, 1> entry_type_count; // 267={0?1}
				entry_t<3, 1> entry_bid; // 269={0?1}
				entry_t<3, 1> entry_offer; // 269={0?1}
				entry_t<3, 1> symbol_count; // 146={0?1}

				// 55={SYMBOL_IS_DIGIT_ONLY ? 0-9:MAX_SYMBOL_DIGITS : 0-9;A-Z:MAX_SYMBOL_DIGITS}
				entry_t<2, settings::broker::MAX_SYMBOL_DIGITS> symbol; 
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

	
	//template<> struct body_t<request::market_data_req> {
	//	using type = body_t<request::market_data_req>;
	//	union {
	//		struct {
	//			entry_t<, > ;
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