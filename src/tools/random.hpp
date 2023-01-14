#pragma once

#include <stdint.h>

#include "concepts.hpp"

namespace ctrader::tools::random{

    using namespace ctrader::tools::concepts;

    namespace internal {

        const char anumLookup[] = 
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "Aa"; // these are used to padd message, so modolu 64 does not return empty charachter if i=63


        consteval uint64_t new_seed() {
            uint64_t shifted = 0;

            for( const auto c : __TIME__ ){
                shifted <<= 8;
                shifted |= c;
            }

            return shifted;
        };

        // first numbers generated as average of outliers in set of random binomial distribution
        // second number is bitshift of compilation time --> this way each sequential compilation will also generated partially new keys. 
        uint64_t seeds[2] = {346786427839U, new_seed()};

    }

    // Based on XorShift128+ algorithm
    static inline __attribute__((always_inline))
    uint64_t rand(){
        uint64_t x = internal::seeds[0];
        uint64_t const y = internal::seeds[1];
        internal::seeds[0] = y;
        x ^= x << 23; // a
        internal::seeds[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
        return internal::seeds[1] + y;
    };

    // Modulo of base 2 is much faster than generic modulo of other numbers. 
    // No loop needed only sigle bit operation
    template<uint8_t N> requires is_power_of_2<N>
    static inline __attribute__((always_inline))
    uint8_t rand_n(){
        return (rand() & (N - 1));
    };


    static inline __attribute__((always_inline))
    char rand_anum(){
        return internal::anumLookup[rand_n<64>()];
    };

}