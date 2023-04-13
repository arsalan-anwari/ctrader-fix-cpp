#pragma once

#include <concepts>
#include <span>

namespace ctrader {

    template<unsigned Offset = 0>
    inline constexpr void to_chars(std::span<char> begin, std::integral auto x) {
        char* end = begin.data() + Offset + begin.size();
        do {
            *--end = static_cast<char>((x % 10) + '0');
            x /= 10;
        } while (x != 0);
    };

    template<unsigned Size, unsigned Offset = 0>
    inline constexpr void to_chars(std::span<char> begin, std::integral auto x) {
        char* end = begin.data() + Offset + Size;
        do {
            *--end = static_cast<char>((x % 10) + '0');
            x /= 10;
        } while (x != 0);
    };

    inline constexpr void to_chars(char*, char* end, std::integral auto x) {
        do {
            *--end = static_cast<char>((x % 10) + '0');
            x /= 10;
        } while (x != 0);
    };

    template<typename T> requires std::unsigned_integral<T>
    inline T to_unsigned_integral(const char* buff, const T size) {
        u64 value = 0;
        u64 multiplier = 1;
        for (T i = size; i > 0; i--) {
            i8 msg_val = (buff[i - 1] - '0');
            value += msg_val * multiplier;
            multiplier = (multiplier << 3) + (multiplier << 1);
        }
        return static_cast<T>(value);
    }

    template<typename T> requires std::unsigned_integral<T>
    inline T to_unsigned_integral(std::string_view buff) {
        u64 value = 0;
        u64 multiplier = 1;
        for (T i = buff.size(); i > 0; i--) {
            i8 msg_val = (buff[i - 1] - '0');
            value += msg_val * multiplier;
            multiplier = (multiplier << 3) + (multiplier << 1);
        }
        return static_cast<T>(value);
    }

}