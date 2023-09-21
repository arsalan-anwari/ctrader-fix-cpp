#pragma once

#include <span>
#include <string_view>

#include "../tools/datetime.hpp"
#include "../types/symbol.hpp"
#include "../settings.hpp"

#include "../tools/algorithm.hpp"

#include "packet_generator.hpp"

namespace {
	using namespace ctrader;

	template<request Tm>
	inline void prepare_header(u64 msg_seq_num, packet_t<Tm>& buff) {
		buff.header.entry.msg_seq_num << msg_seq_num;
		buff.header.entry.sending_time << utc_now();
	}

	template<request Tm>
	inline void prepare_footer(packet_t<Tm>& buff) {
		constexpr unsigned buff_size = static_cast<unsigned>(sizeof(buff.data) - 7);
		const u32 checksum = ascii_sum<buff_size>(buff.data) % 256;
		
		buff.trailer << checksum;
	}
}

namespace ctrader{
namespace encode {

	template<connection Tc, request Tm>
	struct encode_method {
		packet_t<Tm> buff = encode::new_packet<Tm>(Tc);
		void prepare(u64 msg_seq_num) {
			prepare_header(msg_seq_num, buff);
			prepare_footer(buff);
		}
	};

	template<connection Tc>
	struct encode_method<Tc, request::market_data_req> {
		packet_t<request::market_data_req> buff = encode::new_packet<request::market_data_req>(Tc);

		void prepare(
			u64 msg_seq_num,
			std::string_view req_id,
			subscription subs, 
			market_depth depth, 
			symbol sym
		) {
			prepare_header(msg_seq_num, buff);

			buff.body.entry.md_req_id << req_id;
			buff.body.entry.subscription_req_type << subs;
			buff.body.entry.market_depth << depth;
			buff.body.entry.symbol << sym;
			
			prepare_footer(buff);
		}

	};

	//template<connection Tc>
	//struct encode_method<request::logon> {

	//};


}
}

