#pragma once

#include <chrono>
//#include <string_view>
//#include <span>

//#include "strings.hpp"
//#include "convert.hpp"
#include "../types/datetime.hpp"
#include "../types/numbers.hpp"
//#include "../settings.hpp"

namespace ctrader {
	
	//template<u8 Offset>
	//inline void utc_now_32w(std::span<char> out, std::string_view zero_buff) {
	//	static constexpr size_t BuffSize = 32U;
	//	using namespace std::chrono;

	//	// get current datetime from systemclock
	//	auto now = system_clock::now();
	//	auto local_time = local_days{} + (now - sys_days{});
	//	auto local_time_in_days = std::chrono::floor<days>(local_time);

	//	year_month_day ymd{ local_time_in_days };
	//	hh_mm_ss hms{ local_time - local_time_in_days };

	//	// clear buffer
	//	strcpy<BuffSize>(out.data(), zero_buff.data());

	//	// copy content from systemclock to buffer		
	//	to_chars<2, Offset + 0>(out, int{ ymd.year() });
	//	to_chars<2, Offset + 4>(out, unsigned{ ymd.month() });
	//	to_chars<2, Offset + 6>(out, unsigned{ ymd.day() });

	//	to_chars<2, Offset + 9>(out, hms.hours().count());
	//	to_chars<2, Offset + 12>(out, hms.minutes().count());
	//	to_chars<2, Offset + 15>(out, hms.seconds().count());
	//	to_chars<6, Offset + 18>(out, duration_cast<microseconds>(hms.subseconds()).count());
	//}

	inline utc_time_t utc_now() {
		using namespace std::chrono;

		// get current datetime from systemclock
		auto now = system_clock::now();
		auto local_time = local_days{} + (now - sys_days{});
		auto local_time_in_days = std::chrono::floor<days>(local_time);

		year_month_day ymd{ local_time_in_days };
		hh_mm_ss hms{ local_time - local_time_in_days };

		return utc_time_t{
			.year = static_cast<u8>(int{ ymd.year() }),
			.month = static_cast<u8>(unsigned{ ymd.month() }),
			.day = static_cast<u8>(unsigned{ ymd.day() }),
			.hours = static_cast<u8>(hms.hours().count()),
			.minutes = static_cast<u8>(hms.minutes().count()),
			.seconds = static_cast<u8>(hms.seconds().count()),
			.frac_time = static_cast<u32>(duration_cast<microseconds>(hms.subseconds()).count())
		};
	}

}