#pragma once

#include <chrono>

#include "tools/memory.hpp"
#include "tools/numbers.hpp"

namespace ctrader::tools::datetime {

    namespace internal{

         static const char* zero_buff = "|52=00000000-00:00:00.000000|49=";

    }

     inline __attribute__((always_inline))
    void current_timestamp_from_offset( char* out ){
        using namespace std::chrono;
        using namespace ctrader::tools;
        
        auto now = system_clock::now();
        auto local_time = local_days{} + (now - sys_days{});
        auto local_time_in_days = std::chrono::floor<days>(local_time);

        year_month_day ymd{local_time_in_days};
        hh_mm_ss hms{local_time-local_time_in_days};
        
        // clear buffer with zeros to allow signle digits to be represented correct as '05' vs 'n5'
        memory::memcpy_32(out, internal::zero_buff); 

        numbers::to_string(out+4, out+8, int{ymd.year()});
        numbers::to_string(out+8, out+10, unsigned{ymd.month()});
        numbers::to_string(out+10, out+12, unsigned{ymd.day()});

        numbers::to_string(out+13, out+15, hms.hours().count());
        numbers::to_string(out+16, out+18, hms.minutes().count());
        numbers::to_string(out+19, out+21, hms.seconds().count());
        numbers::to_string(out+22, out+28, duration_cast<microseconds>(hms.subseconds()).count());
    }

} //ctrader::tools::datetime