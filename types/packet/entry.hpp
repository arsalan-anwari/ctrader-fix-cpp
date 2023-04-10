#pragma once

#include <iostream> // std::ostream
#include <string>

#include "../message.hpp"
#include "../strings.hpp"
#include "../../tools/json_formatter.hpp"

namespace ctrader {

	template<u8 TagSize, u8 ValueSize, cstring Name>
	struct entry_t {
		using type = entry_t<TagSize, ValueSize, Name>;

		union {
			struct {
				char soh;
				char tag[TagSize];
				char delimeter;
				char value[ValueSize];
			};
			char raw[TagSize + ValueSize + 2];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << json_format::new_group_list(
				Name.value, 
				json_format::new_list( {
					{"Tag", std::string(self.tag, TagSize)}, 
					{"Value", std::string(self.value, ValueSize)}
				})
			);
			return os;
		}
	};

	template<u8 TagSize, u8 ValueSize, cstring Name>
	struct first_entry_t {
		using type = first_entry_t<TagSize, ValueSize, Name>;

		union {
			struct {
				char tag[TagSize];
				char delimeter;
				char value[ValueSize];
			};
			char raw[TagSize + ValueSize + 1];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os << json_format::new_group_list(
				Name.value,
				json_format::new_list({
					{"Tag", std::string(self.tag, TagSize)},
					{"Value", std::string(self.value, ValueSize)}
					})
			);
			return os;
		}
	};

}