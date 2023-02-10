#pragma once

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
        i32 let(i32 a, i32 b){ return ((((b + (~a + 1)) >> 31) & 1) ^ 1); };
        i32 get(i32 a, i32 b){ return (((b + (~a + 1)) >> 31) & 1); };
        i32 ne(i32 a, i32 b){ return !!(a ^ b); }
    }

    template<typename VAL, typename RET = VAL>
    inline __attribute__((always_inline)) 
    consteval RET digit_count(VAL val){
            return static_cast<RET>(std::floor( std::log10( val ) )) + 1;
    }

    inline __attribute__((always_inline)) 
    void overflow_correction(const i64 val, i64& base, u32& digit_size){
        const i64 new_base[3] = { (base * 10U), (base * 10U), base }; 
        const u32 new_digit_size[3] = { (digit_size + 1U), (digit_size + 1U), digit_size };
        i64 res = base - val;
        i64 state = ( (res > 0) - (res < 0) ) + 1;
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
    T to_num(const char* buff);

    template<> u8 to_num<u8, 1>(const char* buff){
        return (buff[0] - '0');
    }

    template<> u8 to_num<u8, 2>(const char* buff){
        return static_cast<u8>( 
            ((buff[0] - '0') * 10U) +
            ((buff[1] - '0'))
        );
    }

    template<> u16 to_num<u16, 3>(const char* buff){
        return static_cast<u16>( 
            ((buff[0] - '0') * 100U) +
            ((buff[1] - '0') * 10U) +
            ((buff[2] - '0'))
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

    template<typename T, T MAX> requires std::integral<T>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    number_info_t<T> to_num_estimate(const char* buff) {
        T msg_size_multiplier = 1;
        T total_msg_size = 0;
        T msg_digit_size = 0;
        for(T i = MAX; i > 0; i--){
            i8 msg_val = (buff[i-1] - '0');
            u8 msg_val_state = (op::get(msg_val, 0) & op::let(msg_val, 9));

            total_msg_size += ( msg_val * msg_val_state) * msg_size_multiplier;
            msg_digit_size += msg_val_state;
            msg_size_multiplier *= (1 + (9 * msg_val_state));
        }

        return {total_msg_size, msg_digit_size};
    }


    template<std::size_t N, typename T> requires std::unsigned_integral<T>
    consteval simple_buffer_t<char, N> to_simple_buffer(T val){
        simple_buffer_t<char, N> buff;
        std::fill_n(buff.data, N, '0');
        to_string(buff.data, buff.data+buff.size, val);
        return buff;
    }


}