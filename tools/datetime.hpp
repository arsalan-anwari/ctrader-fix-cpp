#pragma once

#include <chrono>
#include "../types/datetime.hpp"
#include "../types/numbers.hpp"

namespace ctrader {

	inline utc_time_t utc_now() {
		using namespace std::chrono;

		// get current datetime from systemclock
		auto now = system_clock::now();
		auto local_time = local_days{} + (now - sys_days{});
		auto local_time_in_days = std::chrono::floor<days>(local_time);

		year_month_day ymd{ local_time_in_days };
		hh_mm_ss hms{ local_time - local_time_in_days };

		return utc_time_t{
			.year = static_cast<u16>(int{ ymd.year() }),
			.month = static_cast<u8>(unsigned{ ymd.month() }),
			.day = static_cast<u8>(unsigned{ ymd.day() }),
			.hours = static_cast<u8>(hms.hours().count()),
			.minutes = static_cast<u8>(hms.minutes().count()),
			.seconds = static_cast<u8>(hms.seconds().count()),
			.frac_time = static_cast<u32>(duration_cast<microseconds>(hms.subseconds()).count())
		};
	}

}