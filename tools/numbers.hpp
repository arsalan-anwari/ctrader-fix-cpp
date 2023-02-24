#pragma once

#include <stdio.h>

#include <cmath> // std::log10, std::floor, std::abs
#include <algorithm> // std::fill_n

#include "types/numbers.hpp"
#include "types/concepts.hpp"
#include "tools/memory.hpp"

namespace ctrader::tools::numbers {
    using namespace ctrader::types::concepts;
    using namespace ctrader::types::memory;
    using namespace ctrader::types::numbers;

    template<typename VAL = u32, typename RET = VAL> 
    inline constexpr RET digit_count(VAL val){
        return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

    template<typename T> requires std::integral<T> 
    inline constexpr void to_string(char*, char* end, T x) {
        do{
            *--end = static_cast<char>( (x % 10) + '0' ); 
            x /= 10;
        } while(x != 0);
    }

    template<typename T> requires std::integral<T>
    inline T to_num(const char* buff, const T size){
        T value = 0; 
        u64 multiplier = 1;
        for(T i = size; i > 0; i--){
            i8 msg_val = (buff[i-1] - '0');
            value += msg_val * multiplier;
            multiplier = (multiplier << 3) + (multiplier << 1);
        }
        return value;
    }

    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    consteval simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }


}