#pragma once

#include "packet/header.hpp"
#include "packet/body.hpp"
#include "packet/trailer.hpp"
#include "message.hpp"

namespace ctrader {

	template<message T>
	struct packet_t {
		using type = packet_t<T>;

		union {
			struct {
				header_t header;
				body_t<T> body;
				trailer_t trailer;
			};
			char data[sizeof(header_t) + sizeof(body_t<T>) + sizeof(trailer_t)] = { 0 };
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << '{'
				<< json_format::new_tag("Message", std::string(MESSAGE_NAME[static_cast<u8>(T)])) << ','
				<< self.header << ',' << self.body << ',' << self.trailer
			   << '}';
			return os;
		}


	};

};