#pragma once

#include <string_view>
#include "numbers.hpp"

namespace ctrader {

	enum class request : u8 {
		undefined,
		heart_beat, 
		test_req, 
		logon, 
		logout, 
		resend_req, 
		reject, 
		seq_reset,
		market_data_req, 
		new_order
	};

	enum class response : u8 {
		undefined,
		market_data_snapshot,
		market_data_incremental
	};

	constexpr std::string_view REQUEST_NAME [] = {
		"Undefined Request"
		"Heartbeat", 
		"Test Request",
		"Logon", 
		"Logout",
		"Resend Request", 
		"Reject", 
		"Sequence Reset",
		"Market Data Request", 
		"New Order Single",
	};

	constexpr std::string_view RESPONSE_NAME [] = {
		"Undefined Response",
		"Market Data Snapshot/Full Refresh",
		"Market Data Incremental Refresh"
	};

	constexpr std::string_view REQUEST_ID_VAL [] = {
		"?"
		"0", 
		"1",
		"A", 
		"5",
		"2", 
		"3", 
		"4",
		"V", 
		"D",
	};

	constexpr std::string_view RESPONSE_ID_VAL[] = {
		"?",
		"W",
		"X"
	};

}