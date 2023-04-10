#pragma once

#include <string_view>
#include "numbers.hpp"

namespace ctrader {

	enum class message : u8 {
		heart_beat, 
		test_req, 
		logon, 
		logout, 
		resend_req, 
		reject, 
		seq_reset,
		market_data_snapshot, 
		market_data_incremental,
		market_data_req, 
		new_order,
		undefined
	};

	constexpr std::string_view MESSAGE_NAME[] = {
		"Heartbeat", 
		"Test Request",
		"Logon", 
		"Logout",
		"Resend Request", 
		"Reject", 
		"Sequence Reset",
		"Market Data Snapshot/Full Refresh", 
		"Market Data Incremental Refresh",
		"Market Data Request", 
		"New Order Single",
		"Undefined Message"
	};

	constexpr std::string_view MESSAGE_ID_VAL[] = {
		"0", 
		"1",
		"A", 
		"5",
		"2", 
		"3", 
		"4",
		"W", 
		"X",
		"V", 
		"D",
		"?"
	};

}