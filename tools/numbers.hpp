#pragma once

#include <cmath> // std::log10, std::floor, std::abs
#include <stdint.h>
#include <cstring> // std::memcpy

#include "types/numbers.hpp"

#include "memory.hpp"
#include "concepts"

namespace ctrader::tools::numbers {
    using namespace ctrader::tools::memory;
    using namespace ctrader::tools::concepts;
    using namespace ctrader::types::numbers;

    template<typename VAL, typename RET = VAL>
    inline __attribute__((always_inline)) 
    constexpr RET digit_count(VAL val){
            return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

    inline __attribute__((always_inline)) 
    void overflow_correction(int64_t& val, int64_t& base, uint8_t& digit_size){
        const int64_t new_base[3] = { (base * 10), (base * 10), base }; 
        const uint8_t new_digit_size[3] = { (digit_size + 1U), (digit_size + 1U), digit_size };
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

    template<typename T, T SIZE> 
    requires std::integral<T> && is_minimum_size<SIZE, 1>
    inline __attribute__((always_inline))
    T to_num(const char* buff) {}

    template<> uint8_t to_num<uint8_t, 1>(const char* buff){
        return (buff[0] - '0');
    }

    template<> uint8_t to_num<uint8_t, 2>(const char* buff){
        return static_cast<uint8_t>( 
            ((buff[0] - '0') * 10) +
            ((buff[1] - '0'))
        );
    }

    template<> uint16_t to_num<uint16_t, 3>(const char* buff){
        return static_cast<uint16_t>( 
            ((buff[0] - '0') * 100) +
            ((buff[1] - '0') * 10) +
            ((buff[2] - '0'))
        );
    }

    template<> int64_t to_num<int64_t, 10>(const char* buff){
        return static_cast<int64_t>( 
            ((buff[0] - '0') * 1'000'000'000) +
            ((buff[1] - '0') * 100'000'000) +
            ((buff[2] - '0') * 10'000'000) +
            ((buff[3] - '0') * 1'000'000) +
            ((buff[4] - '0') * 100'000) +
            ((buff[5] - '0') * 10'000) +
            ((buff[6] - '0') * 1'000) +
            ((buff[7] - '0') * 100) +
            ((buff[8] - '0') * 10) +
            ((buff[9] - '0')) 
        );
    }

    template<typename T, T MAX> requires std::integral<T>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    number_info_t<T> to_num_estimate(const char* buff) {
        char msg_size_buff[MAX];
        const T msg_digit_size_lookup[2] = {MAX-1, MAX};

        std::memcpy(msg_size_buff, buff, MAX);

        int8_t msg_digit_last_val = static_cast<int8_t>(msg_size_buff[MAX-1]) - 57;
        T msg_digit_size = msg_digit_size_lookup[
            static_cast<uint8_t>( (msg_digit_last_val > -10) && (msg_digit_last_val <= 0) )
        ];

        T msg_size_multiplier = 1;
        T total_msg_size = 0;
        for(T i = msg_digit_size; i > 0; i--){
            total_msg_size += (msg_size_buff[i-1] - '0') * msg_size_multiplier;
            msg_size_multiplier *= 10;
        }

        return {total_msg_size, msg_digit_size};
    }

    template<> number_info_t<uint16_t> to_num_estimate<uint16_t, 4>(const char* buff) {
        auto last_val = (buff[3] - '0');
        bool last_correct = (last_val > 0) && (last_val < 10);

        uint16_t total_msg_size = static_cast<uint16_t>( 
            ((buff[0] - '0') * 1000) * +
            ((buff[1] - '0') * 100) +
            ((buff[2] - '0') * 10) +
            (last_val * last_correct) 
        );

        return {total_msg_size, 3+last_correct};
    }


    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    constexpr simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }

}