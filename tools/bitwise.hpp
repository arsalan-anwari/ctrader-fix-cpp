#pragma once

#include "../types/numbers.hpp"

namespace ctrader {
namespace bitwise {

    inline i32 negate(i32 x) { return ((x >> 31) | ((~x + 1) >> 31)) + 1; }
    inline i32 lte(i32 a, i32 b) { return ((((b + (~a + 1)) >> 31) & 1) ^ 1); };
    inline i32 gte(i32 a, i32 b) { return (((b + (~a + 1)) >> 31) & 1); };
    inline i32 ne(i32 a, i32 b) { return negate(a ^ b) ^ 1; }
    inline i32 eq(i32 a, i32 b) { return negate(a ^ b); }

}
}