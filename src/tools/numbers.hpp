#pragma once

#include <cmath> // std::log10, std::floor, std::abs
#include <stdint.h>
#include <concepts>

#include "memory.hpp"

namespace ctrader::tools::numbers {
    using namespace ctrader::tools::memory;

    template<typename VAL, typename RET = VAL>
    static inline __attribute__((always_inline)) 
    constexpr RET digit_count(VAL val){
            return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

    static inline __attribute__((always_inline)) 
    bool base_overflow_correction(int64_t& val, int64_t& last_base){
        const int64_t new_base[3] = { (last_base * 10), (last_base * 10), last_base }; 
        const bool result[3] = { true, true, false };
        int64_t res = last_base - val;
        int64_t index = ( (res > 0) - (res < 0) ) + 1;
        last_base = new_base[ index ];
        return result [ index ];
    }

    template<typename T> requires std::integral<T>
    static inline __attribute__((always_inline))
    constexpr void to_string(char*, char* end, T x) {
        do{
            *--end = static_cast<char>( (x % 10) + '0' ); 
            x /= 10;
        } while(x != 0);
    }

    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    constexpr simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }

}