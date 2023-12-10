#pragma once

#include "packet/header.hpp"
#include "packet/body.hpp"
#include "message.hpp"

namespace ctrader {

	template<encode_options Settings, request Request>
	struct packet_t {
		using type = packet_t<Settings, Request>;
		//using value_type = T;

		union {
			struct {
				header<Settings> header;
				body<Settings, Request> body;
				entry<2U, 3U, entry_type::end> trailer;
			};
			char data[sizeof(header<Settings>) + sizeof(body<Settings, Request>) + sizeof(entry<2U, 3U, entry_type::end>)] = {0};
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << self.header << self.body << self.trailer;
			return os;
		}


	};

}