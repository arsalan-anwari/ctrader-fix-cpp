#pragma once

#include <cmath> // std::log10, std::floor, std::abs
#include <stdint.h>

#include "memory.hpp"
#include "concepts"

namespace ctrader::tools::numbers {
    using namespace ctrader::tools::memory;
    using namespace ctrader::tools::concepts;

    template<typename VAL, typename RET = VAL>
     inline __attribute__((always_inline)) 
    constexpr RET digit_count(VAL val){
            return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

     inline __attribute__((always_inline)) 
    void overflow_correction(int64_t& val, int64_t& base, uint16_t& digit_size){
        const int64_t new_base[3] = { (base * 10), (base * 10), base }; 
        const uint16_t new_digit_size[3] = { (digit_size + 1U), (digit_size + 1U), digit_size };
        int64_t res = base - val;
        int64_t state = ( (res > 0) - (res < 0) ) + 1;
        base = new_base[ state ];
        digit_size = new_digit_size [ state ];
    }

    template<typename T> requires std::integral<T>
     inline __attribute__((always_inline))
    constexpr void to_string(char*, char* end, T x) {
        do{
            *--end = static_cast<char>( (x % 10) + '0' ); 
            x /= 10;
        } while(x != 0);
    }

    template<int8_t SIZE> 
    requires is_smaller_than<SIZE, 5> && is_minimum_size<SIZE, 1>
     inline __attribute__((always_inline))
    uint16_t to_num(const char* buff) {
        return static_cast<uint16_t>( 
            ((buff[0] - '0') * 1000) +
            ((buff[1] - '0') * 100) +
            ((buff[2] - '0') * 10) +
            ((buff[3] - '0'))
        );
    }

    template<> uint16_t to_num<1>(const char* buff){
        return (buff[0] - '0');
    }

    template<> uint16_t to_num<2>(const char* buff){
        return static_cast<uint16_t>( 
            ((buff[0] - '0') * 10) +
            ((buff[1] - '0'))
        );
    }

    template<> uint16_t to_num<3>(const char* buff){
        return static_cast<uint16_t>( 
            ((buff[0] - '0') * 100) +
            ((buff[1] - '0') * 10) +
            ((buff[2] - '0'))
        );
    }



    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    constexpr simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }

}