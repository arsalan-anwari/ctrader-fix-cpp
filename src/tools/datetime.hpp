#pragma once

#include <chrono>

#include "tools/memory.hpp"
#include "tools/numbers.hpp"

namespace ctrader::tools::datetime {

    using gmt_offset_t = struct {
            uint8_t year_offset;
            uint8_t month_offset;
            uint8_t day_offset;
            uint8_t hour_offset;
            uint8_t min_offset;
            uint8_t sec_offset;
            uint8_t subsec_offset;
        }; 

    static inline __attribute__((always_inline))
    void current_timestamp_from_offset( char* out, const char* zero_buff, const gmt_offset_t& offset){
        using namespace std::chrono;
        using namespace ctrader::tools;
        
        auto now = system_clock::now();
        auto local_time = local_days{} + (now - sys_days{});
        auto local_time_in_days = std::chrono::floor<days>(local_time);

        year_month_day ymd{local_time_in_days};
        hh_mm_ss hms{local_time-local_time_in_days};
        
        //clear buffer with zeros to allow signle digits to be represented correct as '05' vs 'n5'
        memory::memcpy_32(out, zero_buff); 

        numbers::to_string(out+offset.year_offset, out+offset.year_offset+4, int{ymd.year()});
        numbers::to_string(out+offset.month_offset, out+offset.month_offset+2, unsigned{ymd.month()});
        numbers::to_string(out+offset.day_offset, out+offset.day_offset+2, unsigned{ymd.day()});

        numbers::to_string(out+offset.hour_offset, out+offset.hour_offset+2, hms.hours().count());
        numbers::to_string(out+offset.min_offset, out+offset.min_offset+2, hms.minutes().count());
        numbers::to_string(out+offset.sec_offset, out+offset.sec_offset+2, hms.seconds().count());
        numbers::to_string(out+offset.subsec_offset, out+offset.subsec_offset+6, duration_cast<microseconds>(hms.subseconds()).count());
    }

} //ctrader::tools::datetime