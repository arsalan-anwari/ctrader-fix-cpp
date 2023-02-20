#pragma once

#include <stdio.h>

#include <cmath> // std::log10, std::floor, std::abs
#include <algorithm> // std::fill_n

#include "types/numbers.hpp"

#include "memory.hpp"
#include "concepts"

namespace ctrader::tools::numbers {
    using namespace ctrader::tools::memory;
    using namespace ctrader::tools::concepts;
    using namespace ctrader::types::numbers;

    namespace op {
        inline __attribute__((always_inline)) i32 lte(i32 a, i32 b){ return ((((b + (~a + 1)) >> 31) & 1) ^ 1); };
        inline __attribute__((always_inline)) i32 gte(i32 a, i32 b){ return (((b + (~a + 1)) >> 31) & 1); };
        inline __attribute__((always_inline)) i32 negate(i32 x){ return ((x >> 31) | ((~x + 1) >> 31)) + 1; }
        inline __attribute__((always_inline)) i32 ne(i32 a, i32 b){ return negate(a ^ b) ^ 1; }
    }

    consteval u32 min_permutes( const u32 size ){
        i32 remainder = size;
        u32 len = 0;
        while( remainder > 0 ){
            remainder -= 8;
            len++;
        }

        return len;
    }

    template<typename VAL, typename RET = VAL>
    inline __attribute__((always_inline)) 
    consteval RET digit_count(VAL val){
            return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

    template<typename T> requires std::integral<T>
    inline __attribute__((always_inline))
    constexpr void to_string(char*, char* end, T x) {
        do{
            *--end = static_cast<char>( (x % 10) + '0' ); 
            x /= 10;
        } while(x != 0);
    }

    template<typename T, T N>
    requires std::integral<T> && is_minimum_size<N, 1>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    T to_num_estimate(const char* buff) {
        T msg_size_multiplier = 1;
        T total_msg_size = 0;

        for(T i = N; i > 0; i--){
            i32 msg_val = (buff[i-1] - '0');
            i32 msg_val_state = (op::gte(msg_val, 0) & op::lte(msg_val, 9));
            u32 msg_val_state_mask = 0 - msg_val_state;

            total_msg_size += msg_val * (msg_size_multiplier & msg_val_state_mask);
            msg_size_multiplier = (msg_size_multiplier << (3 & msg_val_state_mask) ) + (( msg_size_multiplier << 1 ) & msg_val_state_mask);
        }

        return total_msg_size;
    }

    template<typename T, T N>
    requires std::integral<T> && is_minimum_size<N, 1>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    T to_digit_size(const char* buff) {
        T digit_size = 0;

        for(T i = 0; i < N; i++){
            i32 msg_val = (buff[i] - '0');
            u32 msg_val_state = (op::gte(msg_val, 0) & op::lte(msg_val, 9));

            digit_size += msg_val_state;
        }

        return digit_size;
    }

    template<typename T, T N>
    requires std::integral<T> && is_minimum_size<N, 1>
    inline __attribute__((always_inline))
    T to_num(const char* buff);

    template<> u32 to_num<u32, 2>(const char* buff){
        return static_cast<u32>( 
            ((buff[0] - '0') * 10U) +
            ((buff[1] - '0'))
        );
    }

    template<> i64 to_num<i64, 10>(const char* buff){
        return static_cast<i64>( 
            ((buff[0] - '0') * 1'000'000'000U) +
            ((buff[1] - '0') * 100'000'000U) +
            ((buff[2] - '0') * 10'000'000U) +
            ((buff[3] - '0') * 1'000'000U) +
            ((buff[4] - '0') * 100'000U) +
            ((buff[5] - '0') * 10'000U) +
            ((buff[6] - '0') * 1'000U) +
            ((buff[7] - '0') * 100U) +
            ((buff[8] - '0') * 10U) +
            ((buff[9] - '0')) 
        );
    }

    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    consteval simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }


}