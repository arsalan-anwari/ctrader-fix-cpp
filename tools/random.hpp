#pragma once

#include "types/numbers.hpp"
#include "concepts.hpp"

namespace ctrader::tools::random{

    using namespace ctrader::types::numbers;
    using namespace ctrader::types::concepts;

    namespace {

        const char anumLookup[] = 
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "Aa"; // these are used to padd message, so modolu 64 does not return empty charachter if i=63


        consteval u64 new_seed() {
            u64 shifted = 0;

            for( const auto c : __TIME__ ){
                shifted <<= 8;
                shifted |= c;
            }

            return shifted;
        };

        // first numbers generated as average of outliers in set of random binomial distribution
        // second number is bitshift of compilation time --> this way each sequential compilation will also generated partially new keys. 
        u64 seeds[2] = {346786427839U, new_seed()};

    }

    // Based on XorShift128+ algorithm
     inline __attribute__((always_inline))
    u64 rand(){
        u64 x = seeds[0];
        u64 const y = seeds[1];
        seeds[0] = y;
        x ^= x << 23; // a
        seeds[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
        return seeds[1] + y;
    };

    // Modulo of base 2 is much faster than generic modulo of other numbers. 
    // No loop needed only sigle bit operation
    template<u8 N> requires is_power_of_2<N>
     inline __attribute__((always_inline))
    u8 rand_n(){
        return (rand() & (N - 1));
    };


     inline __attribute__((always_inline))
    char rand_anum(){
        return anumLookup[rand_n<64>()];
    };

}