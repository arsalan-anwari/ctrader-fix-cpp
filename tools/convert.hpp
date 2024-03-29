#pragma once

#include <concepts>
#include <span>
#include <memory>

#include "../types/price.hpp"
#include "../types/datetime.hpp"
#include "../types/concepts.hpp"
#include "../types/string.hpp"

#include "../debug_settings.hpp"

#include "algorithm.hpp"
#include "math.hpp"

namespace ctrader {

    inline constexpr void from_intergral(std::span<char> out, std::integral auto value) {
        char* end = out.data() + out.size();
        
        do {
            *--end = static_cast<char>((value % 10) + '0');
            value /= 10;
        } while (value != 0);
    }

    inline void from_utc_time(
        std::span<char> out, const std::string_view date_time_mask, 
        const utc_time& time, const utc_time_offset offset
    ) {
        using namespace std::chrono;

        // clear buffer with datetime_mask
        std::memcpy(out.data(), date_time_mask.data(), date_time_mask.size());
        
        // copy time to buffer
        from_intergral(out.subspan(offset.year, 4), time.year);
        from_intergral(out.subspan(offset.month, 2), time.month);
        from_intergral(out.subspan(offset.day, 2), time.day);

        from_intergral(out.subspan(offset.hours, 2), time.hours);
        from_intergral(out.subspan(offset.minutes, 2), time.minutes);
        from_intergral(out.subspan(offset.seconds, 2), time.seconds);
        from_intergral(out.subspan(offset.frac_time, 6), time.frac_time);
    }

    template<typename T> requires std::integral<T>
    inline T as_integral(std::string_view buff) {
        u64 value = 0;
        u64 multiplier = 1;
        const T size = static_cast<T>(buff.size());
        for (T i = size; i > 0; i--) {
            i8 msg_val = (buff[i - 1] - '0');
            value += msg_val * multiplier;
            multiplier = (multiplier << 3) + (multiplier << 1);
        }
        return static_cast<T>(value);
    }

    inline price as_price(std::string_view buff) {
        u32 pos_d = find<16U>(buff.data(), '.');
        u32 size_frac = static_cast<u32>(buff.size() - (pos_d + 1U));

        return price{
            as_integral<i32>(buff.substr(0, pos_d)),
            as_integral<u32>(buff.substr(pos_d + 1, size_frac)),
            static_cast<u8>(rfind<5U>(buff.data() + pos_d + 1, '0'))
        };
    }

    template<usize value>
    consteval cv_string<cv_digit_count(value)> as_cv_string() {
        constexpr auto value_digits = cv_digit_count(value);
        cv_string<value_digits> buff;

        from_intergral(std::span(buff.data, value_digits), value);

        return buff;
    }


}