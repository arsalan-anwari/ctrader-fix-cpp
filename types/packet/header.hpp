#pragma once

#include "entry.hpp"
#include "../../debug_settings.hpp"
#include "../../tools/math.hpp"
#include "../../encode_settings.hpp"



namespace ctrader {
	
	template<encode_options Settings>
	struct header {

		union {
			struct {
				entry<1U, string_length(Settings.fix_version), entry_type::start> begin_string; // 8={FIX_VERSION}
				entry<1U, Settings.max_msg_size_digits> body_length; // |9={0-9:MAX_MSG_SIZE_DIGITS}
				entry<2U, Settings.max_msg_type_digits> msg_type; // |35={0-9;A-Z:MAX_MSG_TYPE_DIGITS}
				entry<2U, string_length(Settings.sender_comp_id)> sender_comp_id; // |49={SENDER_COMP_ID}
				entry<2U, string_length(Settings.target_comp_id)> target_comp_id; // |56={TARGET_COMP_ID}
				entry<2U, string_length(Settings.target_sub_id[0])> target_sub_id; //|57={{0-9;A-Z}->TARGET_SUB_ID}
				entry<2U, Settings.max_seq_num_digits> msg_seq_num; // |34={0-9:MAX_SEQ_NUM_DIGITS}
				entry<2U, string_length(Settings.date_time_mask)> sending_time; // |52={DATE_TIME_MASK}
				
			} entry;
			char raw[sizeof(entry)];
		};

		friend std::ostream& operator<<(std::ostream& os, const header& self) {
			os
				<< self.entry.begin_string
				<< self.entry.body_length
				<< self.entry.msg_type
				<< self.entry.sender_comp_id
				<< self.entry.target_comp_id
				<< self.entry.target_sub_id
				<< self.entry.msg_seq_num
				<< self.entry.sending_time;
			
			return os;
		}


	};

}