#pragma once

#include <format>

#include "entry.hpp"
#include "../../settings.hpp"

namespace ctrader {

	struct header_t {

		union {
			struct {
				first_entry_t<1, 7, "BeginString"> begin_string; // 8=FIX.4.4
				entry_t<1, 3, "BodyLength"> body_length; // |9={0-9:3}
				entry_t<2, 1, "MsgType"> msg_type; // |35={0-9;A-Z:1}
				entry_t<2, settings::MAX_SEQ_NUM_DIGITS, "MsgSeqNum"> msg_seq_num; // |34={0-9:MAX_SEQ_NUM_DIGITS}
				entry_t<2, 24, "SendingTime"> sending_time; // |52={yyyymmdd-HH:MM:SS.6f}
				entry_t<2, settings::broker::SENDER_COMP_ID.size(), "SenderCompId"> sender_comp_id; // |49={SENDER_COMP_ID}
				entry_t<2, 7, "TargetCompId"> target_comp_id; // |56=cServer
				entry_t<2, 5, "TargetSubId"> target_sub_id; //|57={QUOTE?TRADE}
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const header_t& self) {
			os << std::format("\"{}\"", "Header") << ":{"
				<< self.entry.begin_string << ','
				<< self.entry.body_length << ','
				<< self.entry.msg_type << ','
				<< self.entry.msg_seq_num << ','
				<< self.entry.sending_time << ','
				<< self.entry.sender_comp_id << ','
				<< self.entry.target_comp_id << ','
				<< self.entry.target_sub_id
			   << "}";
			return os;
		}


	};

}