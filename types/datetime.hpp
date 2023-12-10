#pragma once

#include <iomanip> // std::setfill
#include <iostream> // std::ostream
#include "numbers.hpp"

namespace ctrader {

    struct utc_time {
        u16 year; u8 month; u8 day;
        u8 hours; u8 minutes; u8 seconds;
        u32 frac_time;
    
        friend std::ostream& operator<<(std::ostream& os, const utc_time& self) {
            os
                << std::setfill('0') << std::setw(4) << static_cast<i32>(self.year)
                << std::setfill('0') << std::setw(2) << static_cast<i32>(self.month)
                << std::setfill('0') << std::setw(2) << static_cast<i32>(self.day) << "-"
                << std::setfill('0') << std::setw(2) << static_cast<i32>(self.hours) << ":"
                << std::setfill('0') << std::setw(2) << static_cast<i32>(self.minutes) << ":"
                << std::setfill('0') << std::setw(2) << static_cast<i32>(self.seconds) << "."
                << std::setfill('0') << std::setw(6) << self.frac_time;
            
            return os;
        }

    };

    using utc_time_offset = struct {
        u8 year, month, day;
        u8 hours, minutes, seconds, frac_time;
    };

    static constexpr auto default_utc_time_offset = utc_time_offset{
        .year = 0u, .month = 4u, .day = 6u,
        .hours = 9u, .minutes = 12u, .seconds = 15u, .frac_time = 18u
    };

    static constexpr std::string_view default_date_time_mask = "00000000-00:00:00.000000";
    



};