#pragma once

#include <concepts>
#include <span>
#include <memory>

#include "../types/price.hpp"
#include "../types/datetime.hpp"
#include "../types/concepts.hpp"

#include "../settings.hpp"

#include "algorithm.hpp"

namespace ctrader {

    inline constexpr void from_intergral(std::span<char> out, std::integral auto value) {
        char* end = out.data() + out.size();
        
        do {
            *--end = static_cast<char>((value % 10) + '0');
            value /= 10;
        } while (value != 0);
    }

    inline void from_utc_time(std::span<char> out, const utc_time_t& time) {
        using namespace std::chrono;

        // clear buffer with datetime_mask
        std::memcpy(out.data(), settings::DATE_TIME_MASK.data(), settings::DATE_TIME_MASK.size());
        
        // copy time to buffer
        from_intergral(out.subspan(0, 2), time.year);
        from_intergral(out.subspan(4, 2), time.month);
        from_intergral(out.subspan(6, 2), time.day);

        from_intergral(out.subspan(9, 2), time.hours);
        from_intergral(out.subspan(12, 2), time.minutes);
        from_intergral(out.subspan(15, 2), time.seconds);
        from_intergral(out.subspan(18, 6), time.frac_time);
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

    inline price_t as_price(std::string_view buff) {
        u32 pos_d = find<16U>(buff.data(), '.');
        u32 size_frac = static_cast<u32>(buff.size() - (pos_d + 1U));

        return price_t{
            as_integral<i32>(buff.substr(0, pos_d)),
            as_integral<u32>(buff.substr(pos_d + 1, size_frac)),
            static_cast<u8>(rfind<5U>(buff.data() + pos_d + 1, '0'))
        };
    }


}