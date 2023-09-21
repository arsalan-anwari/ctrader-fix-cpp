#pragma once

#include <iostream> // std::ostream
#include "numbers.hpp"

namespace ctrader {

    struct utc_time_t {
        u16 year; u8 month; u8 day;
        u8 hours; u8 minutes; u8 seconds;
        u32 frac_time;
    
        friend std::ostream& operator<<(std::ostream& os, const utc_time_t& self) {
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



};