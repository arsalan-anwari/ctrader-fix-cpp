#pragma once

#include <concepts>
#include <span>

#include "find.hpp"
#include "../types/price.hpp"


namespace ctrader {

    template<unsigned Offset = 0>
    inline constexpr void from_intergral(std::span<char> begin, std::integral auto x) {
        char* end = begin.data() + Offset + begin.size();
        do {
            *--end = static_cast<char>((x % 10) + '0');
            x /= 10;
        } while (x != 0);
    };

    //inline constexpr void to_chars(char*, char* end, std::integral auto x) {
    //    do {
    //        *--end = static_cast<char>((x % 10) + '0');
    //        x /= 10;
    //    } while (x != 0);
    //};

    template<typename T> requires std::integral<T>
    inline T as_integral(std::string_view buff) {
        u64 value = 0;
        u64 multiplier = 1;
        for (T i = buff.size(); i > 0; i--) {
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
            static_cast<u8>(find_end<5U>(buff.data() + pos_d + 1, '0'))
        };
    }

}