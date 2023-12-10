#pragma once

#include <span>
#include <string_view>

#include "../tools/datetime.hpp"
#include "../types/symbol.hpp"
#include "../debug_settings.hpp"

#include "../tools/algorithm.hpp"

#include "packet_generator.hpp"

namespace {
	using namespace ctrader;

	template<encode_options Settings, request RequestType>
	inline void prepare_header(u64 msg_seq_num, packet_t<Settings, RequestType>& buff) {
		buff.header.entry.msg_seq_num << msg_seq_num;
		buff.header.entry.sending_time << utc_now();
	}

	template<encode_options Settings, request RequestType>
	inline void prepare_footer(packet_t<Settings, RequestType>& buff) {
		constexpr unsigned buff_size = static_cast<unsigned>(sizeof(buff.data) - 7);
		const u32 checksum = ascii_sum<buff_size>(buff.data) % 256;
		
		buff.trailer << checksum;
	}
}

namespace ctrader{
namespace encode {

	template<connection ConnectionType, encode_options Settings, request RequestType>
	struct encode_method {
		packet_t<Settings, RequestType> buff = encode::packet_generator<ConnectionType, Settings, RequestType>::data;
		void prepare(u64 msg_seq_num) {
			prepare_header(msg_seq_num, buff);
			prepare_footer(buff);
		}
	};

	template<connection ConnectionType, encode_options Settings>
	struct encode_method<ConnectionType, Settings, request::market_data_req> {
		packet_t<Settings, request::market_data_req> buff = encode::packet_generator<ConnectionType, Settings, request::market_data_req>::data;

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

