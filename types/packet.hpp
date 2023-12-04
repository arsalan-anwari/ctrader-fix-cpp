#pragma once

#include "packet/header.hpp"
#include "packet/body.hpp"
#include "message.hpp"

namespace ctrader {

	template<encode::encode_settings_t Settings, request Request>
	struct packet_t {
		using type = packet_t<Settings, Request>;
		//using value_type = T;

		union {
			struct {
				header_t<Settings> header;
				body_t<Settings, Request> body;
				entry_t<2U, 3U, entry_type_t::end> trailer;
			};
			char data[sizeof(header_t<Settings>) + sizeof(body_t<Settings, Request>) + sizeof(entry_t<2U, 3U, entry_type_t::end>)] = {0};
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << self.header << self.body << self.trailer;
			return os;
		}


	};

}