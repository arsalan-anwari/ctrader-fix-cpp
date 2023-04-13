#pragma once

#include <iostream>

#include "../numbers.hpp"
#include "../../tools/format.hpp"

namespace ctrader {

	struct trailer_t {
		using type = trailer_t;
		union {
			struct {
				char soh_start;
				char tag[2];
				char delimeter;
				char value[3];
				char soh_end;
			};
			char raw[1 + 2 + 7];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << json_format::new_subgroup(
				"Trailer",
				json_format::new_subgroup(
					"Checksum",
					json_format::new_list({
						{"Tag", std::string(self.tag, sizeof(self.tag))},
						{"Value", std::string(self.value, sizeof(self.value))}
					})
				)
			);
			return os;
		}
	};

}