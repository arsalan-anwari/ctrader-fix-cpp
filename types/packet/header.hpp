#pragma once

#include <format>

#include "entry.hpp"
#include "../../settings.hpp"

namespace ctrader {

	struct header_t {

		union {
			struct {
				entry_t<1, settings::broker::FIX_VERSION.size(), entry_type_t::start> begin_string; // 8={FIX_VERSION}
				entry_t<1, settings::broker::MAX_MSG_SIZE_DIGITS> body_length; // |9={0-9:MAX_MSG_SIZE_DIGITS}
				entry_t<2, settings::broker::MAX_MSG_TYPE_DIGITS> msg_type; // |35={0-9;A-Z:MAX_MSG_TYPE_DIGITS}
				entry_t<2, settings::MAX_SEQ_NUM_DIGITS> msg_seq_num; // |34={0-9:MAX_SEQ_NUM_DIGITS}
				entry_t<2, settings::broker::DATE_TIME_FMT.size()> sending_time; // |52={DATE_TIME_FMT}
				entry_t<2, settings::broker::SENDER_COMP_ID.size()> sender_comp_id; // |49={SENDER_COMP_ID}
				entry_t<2, settings::broker::TARGET_COMP_ID.size()> target_comp_id; // |56={TARGET_COMP_ID}
				entry_t<2, TARGET_SUB_ID[0].size()> target_sub_id; //|57={{0-9;A-Z}->TARGET_SUB_ID}
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const header_t& self) {
			os
				<< self.entry.begin_string
				<< self.entry.body_length
				<< self.entry.msg_type
				<< self.entry.msg_seq_num
				<< self.entry.sending_time
				<< self.entry.sender_comp_id
				<< self.entry.target_comp_id
				<< self.entry.target_sub_id;
			
			return os;
		}


	};

}