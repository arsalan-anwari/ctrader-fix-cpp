#pragma once

#include <iostream> // std::ostream
#include <string> // std::string_view, std::span
#include <concepts> // std::integral
#include <memory>
#include <tuple> // std::tuple

#include "../encode.hpp"
#include "../symbol.hpp"
#include "../message.hpp"

#include "../../tools/convert.hpp"
#include "../../tools/datetime.hpp"


namespace ctrader {

	enum class entry_type { start, normal, end };

	template<u8 TagSize, u8 ValueSize, entry_type Type = entry_type::normal>
	struct entry {
		using type = entry<TagSize, ValueSize, entry_type::normal>;

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
			from_intergral(std::span<char>(self.value), val);
			return self;
		}

		friend type& operator<<(type& self, const utc_time& time) {
			from_utc_time(std::span<char>(self.value), default_date_time_mask, time, default_utc_time_offset);
			return self;
		}

		friend type& operator<<(type& self, const std::tuple<utc_time, utc_time_offset> & time_data) {
			const auto& [time, offset] = time_data;
			from_utc_time(std::span<char>(self.value), default_date_time_mask, time, offset);
			return self;
		}

		friend type& operator<<(type& self, const std::tuple<std::string_view, utc_time, utc_time_offset>& time_data) {
			const auto& [mask, time, offset] = time_data;
			from_utc_time(std::span<char>(self.value), mask, time, offset);
			return self;
		}

		friend type& operator<<(type& self, std::string_view val) {
			std::memcpy(self.value, val.data(), val.size());
			return self;
		}

		friend type& operator<<(type& self, subscription subs) {
			self.value[0] = static_cast<char>(subs);
			return self;
		}

		friend type& operator<<(type& self, market_depth depth) {
			self.value[0] = static_cast<char>(depth);
			return self;
		}

		friend type& operator<<(type& self, symbol sym) {
			from_intergral(std::span<char>(self.value), static_cast<u64>(sym));
			return self;
		}

	};


	template<u8 TagSize, u8 ValueSize>
	struct entry<TagSize, ValueSize, entry_type::start> {
		using type = entry<TagSize, ValueSize, entry_type::start>;

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

		friend type& operator<<(type& self, std::string_view val) {
			std::memcpy(self.value, val.data(), val.size());
			return self;
		}

	};

	template<u8 TagSize, u8 ValueSize>
	struct entry<TagSize, ValueSize, entry_type::end> {
		using type = entry<TagSize, ValueSize, entry_type::end>;

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
			std::memset(self.value, '0', 3);
			from_intergral(std::span<char>(self.value), val);
			return self;
		}

	};

}