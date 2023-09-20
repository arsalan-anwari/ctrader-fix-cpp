#pragma once

#include "packet/header.hpp"
#include "packet/body.hpp"
#include "packet/trailer.hpp"
#include "message.hpp"

namespace ctrader {

	template<request T>
	struct packet_t {
		using type = packet_t<T>;

		union {
			struct {
				header_t header;
				body_t<T> body;
				entry_t<entry_type_t::end> trailer;
			};
			char data[sizeof(header_t) + sizeof(body_t<T>) + sizeof(entry_t<entry_type_t::end>) = {0};
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << header << body << trailer;
			return os;
		}


	};

};