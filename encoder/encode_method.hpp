#pragma once

#include <span>

#include "packet_generator.hpp"

#include "../tools/datetime.hpp"
#include "../types/symbol.hpp"
#include "../settings.hpp"

namespace {
	using namespace ctrader;

	template<message Tm>
	inline void prepare_header(u64 msg_seq_num, packet_t<Tm>& buff) {
		to_chars(
			std::span<char>(buff.header.entry.msg_seq_num.value),
			msg_seq_num
		);

		utc_now<32, 4>(std::span<char>(buff.header.entry.sending_time.raw), settings::DATE_TIME_MASK);
	}

	template<message Tm>
	inline void prepare_footer(packet_t<Tm>& buff) {
		const u32 checksum = ascii_sum<sizeof(buff.data) - 7>(buff.data) % 256;
		std::memset(buff.trailer.value, '0', 3);
		to_chars( std::span<char>(buff.trailer.value), checksum );
	}
}

namespace ctrader{
namespace encode {

	template<connection Tc, message Tm>
	struct encode_method {
		packet_t<Tm> buff = encode::new_packet<Tm>(Tc);
		void prepare(u64 msg_seq_num) {
			prepare_header(msg_seq_num, buff);
			prepare_footer(buff);
		}
	};

	template<connection Tc>
	struct encode_method<Tc, message::market_data_req> {
		packet_t<message::market_data_req> buff = encode::new_packet<message::market_data_req>(Tc);

		void prepare(
			u64 msg_seq_num,
			std::string_view req_id,
			subscription subs, 
			market_depth depth, 
			symbol sym
		) {
			prepare_header(msg_seq_num, buff);

			strcpy<static_cast<unsigned>(settings::MAX_REQ_ID_DIGITS)>(
				buff.body.entry.md_req_id.value,
				req_id.data()
			);

			buff.body.entry.subscription_req_type.value[0] = static_cast<char>(subs);
			buff.body.entry.market_depth.value[0] = static_cast<char>(depth);

			to_chars(std::span<char>(buff.body.entry.symbol.value), static_cast<u64>(sym));
			
			prepare_footer(buff);
		}

	};

	//template<connection Tc>
	//struct encode_method<message::logon> {

	//};


}
}

