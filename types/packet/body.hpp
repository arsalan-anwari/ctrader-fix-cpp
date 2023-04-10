#pragma once

#include <format>

#include "entry.hpp"
#include "../../tools/numbers.hpp"

namespace ctrader {

	template<message T> struct body_t {
		using type = body_t<T>;
		union {
			struct {
				entry_t<3, 10, "TestReqId"> test_req_id;
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << std::format("\"{}\"", "Body") << ":{" << self.entry.test_req_id << "}";
			return os;
		}
	};

	template<> struct body_t<message::logon> {
		using type = body_t<message::logon>;
		union {
			struct {
				entry_t<2, 1, "EncryptMethod"> encrypt_method;
				entry_t<3, settings::HEARTBEAT_SEC.size(), "HeartBtInt"> heartbeat;
				entry_t<3, 1, "ResetSeqNumFlag"> reset_seq_num_flag;
				entry_t<3, settings::broker::USER_NAME.size(), "Username"> username;
				entry_t<3, settings::broker::PASSWORD.size(), "Password"> password;
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << std::format("\"{}\"", "Body") << ":{" 
				<< self.entry.encrypt_method << ","
				<< self.entry.heartbeat << ","
				<< self.entry.reset_seq_num_flag << ","
				<< self.entry.username << ","
				<< self.entry.password
			   << "}";
			return os;
		}
	};

	template<> struct body_t<message::market_data_req> {
		using type = body_t<message::market_data_req>;
		union {
			struct {
				entry_t<3, settings::MAX_REQ_ID_DIGITS, "MDReqID"> md_req_id;
				entry_t<3, 1, "SubscriptionRequestType"> subscription_req_type;
				entry_t<3, 1, "MarketDepth"> market_depth;
				entry_t<3, 1, "MDUpdateType"> md_update_type;
				entry_t<3, 1, "NoMDEntryTypes"> entry_type_count;
				entry_t<3, 1, "MDEntryType.Bid"> entry_bid;
				entry_t<3, 1, "MDEntryType.Offer"> entry_offer;
				entry_t<3, 1, "NoRelatedSym"> symbol_count;
				entry_t<2, 19, "Symbol"> symbol;
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << std::format("\"{}\"", "Body") << ":{"
				<< self.entry.md_req_id << ","
				<< self.entry.subscription_req_type << ","
				<< self.entry.market_depth << ","
				<< self.entry.md_update_type << ","
				<< self.entry.entry_type_count << ","
				<< self.entry.entry_bid << ","
				<< self.entry.entry_offer << ","
				<< self.entry.symbol_count << ","
				<< self.entry.symbol
			   << "}";
			return os;
		}
	};

	
	//template<> struct body_t<message::market_data_req> {
	//	using type = body_t<message::market_data_req>;
	//	union {
	//		struct {
	//			
	//		} entry;
	//		char raw[sizeof(entry)];
	//	};

	//	friend std::ostream& operator<<(std::ostream& os, const type& self) {
	//		os << std::format("\"{}\"", "Body") << ":{"
	//			<< self.entry. << ","
	//			<< "}";
	//		return os;
	//	}
	//};

}