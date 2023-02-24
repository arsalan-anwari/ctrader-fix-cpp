#pragma once

#include <cstdint>

namespace ctrader::types::numbers {

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint_fast32_t;
    using u64 = uint64_t;
    using u128 = __uint128_t;

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int_fast32_t;
    using i64 = int64_t;
    using i128 = __int128_t;

    namespace op {
        inline i32 lte(i32 a, i32 b){ return ((((b + (~a + 1)) >> 31) & 1) ^ 1); };
        inline i32 gte(i32 a, i32 b){ return (((b + (~a + 1)) >> 31) & 1); };
        inline i32 negate(i32 x){ return ((x >> 31) | ((~x + 1) >> 31)) + 1; }
        inline i32 ne(i32 a, i32 b){ return negate(a ^ b) ^ 1; }
        inline i32 eq(i32 a, i32 b){ return negate(a ^ b); }
    }
    
} // ctrader::types::numbers