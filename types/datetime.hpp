#pragma once

#include <iostream> // std::ostream
#include "numbers.hpp"

namespace ctrader {

    struct utc_time_t {
        u8 year; u8 month; u8 day;
        u8 hours; u8 minutes; u8 seconds;
        u32 frac_time;
    
        friend std::ostream& operator<<(std::ostream& os, const utc_time_t& self) {
            os
                << static_cast<i32>(self.year)
                << static_cast<i32>(self.month)
                << static_cast<i32>(self.day) << "-"
                << static_cast<i32>(self.hours) << ":"
                << static_cast<i32>(self.minutes) << ":"
                << static_cast<i32>(self.seconds) << "."
                << self.frac_time;
            
            return os;
        }

    };



};