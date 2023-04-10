#pragma once

#include <chrono>
#include <string_view>
#include <format>
#include <cstring>
#include <span>

#include "strings.hpp"
#include "type_converter.hpp"
#include "../types/concepts.hpp"
#include "../settings.hpp"

namespace ctrader {
	
	template<size_t BuffSize, u8 Offset> requires concepts::is_min_size<BuffSize+Offset, 24>
	inline void utc_now(std::span<char> out, std::string_view zero_buff) {
		using namespace std::chrono;

		// get current datetime from systemclock
		auto now = system_clock::now();
		auto local_time = local_days{} + (now - sys_days{});
		auto local_time_in_days = std::chrono::floor<days>(local_time);

		year_month_day ymd{ local_time_in_days };
		hh_mm_ss hms{ local_time - local_time_in_days };

		// clear buffer
		strcpy<BuffSize>(out, zero_buff);

		// copy content from systemclock to buffer		
		to_chars<2, Offset + 0>(out, int{ ymd.year() });
		to_chars<2, Offset + 4>(out, unsigned{ ymd.month() });
		to_chars<2, Offset + 6>(out, unsigned{ ymd.day() });

		to_chars<2, Offset + 9>(out, hms.hours().count());
		to_chars<2, Offset + 12>(out, hms.minutes().count());
		to_chars<2, Offset + 15>(out, hms.seconds().count());
		to_chars<6, Offset + 18>(out, duration_cast<microseconds>(hms.subseconds()).count());
	}

}