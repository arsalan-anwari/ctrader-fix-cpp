#pragma once

#include <cstdint>
#include <concepts>

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


} // ctrader::types::numbers