#pragma once

#include <iostream> // std::ostream
#include <string> // std::string_view, std::span
#include <concepts> // std::integral

#include "../message.hpp"
#include "../../tools/convert.hpp"
#include "../../tools/datetime.hpp"

namespace ctrader {

	enum class entry_type_t { start, normal, end };

	template<u8 TagSize = 2U, u8 ValueSize = 3U, entry_type_t Type = entry_type_t::normal>
	struct entry_t {
		using type = entry_t<TagSize, ValueSize, entry_type_t::normal>;

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
			os
				<< self.soh << std::string_view(self.tag, TagSize)
				<< self.delimeter << std::string_view(self.value, ValueSize);
			return os;
		}

		friend type& operator<<(type& self, std::integral auto val) {
			to_chars(std::span<char>(self.value), val);
			return self;
		}

		friend type& operator<<(type& self, const utc_time_t& time) {

		}

	};


	template<u8 TagSize, u8 ValueSize>
	struct entry_t< TagSize, ValueSize, entry_type_t::start > {
		using type = entry_t<TagSize, ValueSize, entry_type_t::start>;

		union {
			struct {
				char tag[TagSize];
				char delimeter;
				char value[ValueSize];
			};
			char raw[TagSize + ValueSize + 1];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os
				<< std::string_view(self.tag, TagSize)
				<< self.delimeter << std::string_view(self.value, ValueSize);
			return os;
		}

		friend type& operator<<(type& self, std::integral auto val) {
			to_chars(std::span<char>(self.value), val);
			return self;
		}
	};

	template<u8 TagSize, u8 ValueSize>
	struct entry_t<TagSize, ValueSize, entry_type_t::end > {
		using type = entry_t<TagSize, ValueSize, entry_type_t::end>;

		union {
			struct {
				char soh_start;
				char tag[TagSize];
				char delimeter;
				char value[ValueSize];
				char soh_end;
			};
			char raw[TagSize + ValueSize + 3];
		};

		friend std::ostream& operator<<(std::ostream& os, const type& self) {
			os
				<< self.soh_start << std::string_view(self.tag, TagSize)
				<< self.delimeter << std::string_view(self.value, ValueSize)
				<< self.soh_end;
			return os;
		}

		friend type& operator<<(type& self, std::integral auto val) {
			to_chars(std::span<char>(self.value), val);
			return self;
		}
	};

}