#pragma once

#include <x86intrin.h>

#include "types/numbers.hpp"
#include "tools/concepts.hpp"

namespace ctrader::parser {

    using namespace ctrader::tools::concepts;
    using namespace ctrader::types::numbers;

    template<std::size_t SIZE, u8 REM = SIZE % 32> requires is_minimum_size<SIZE, 32>
     inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    u8 calc_checksum( const char* data ){
        const __m256i zeroVec = _mm256_setzero_si256();
        const __m256i oneVec = _mm256_set1_epi16(1);
        __m256i accum = _mm256_setzero_si256();

        unsigned int remainder = 0;
        for(u8 i=0; i < REM; i++) { remainder += static_cast<unsigned int>(data[i]); }

        for(std::size_t offset = 0; offset <= SIZE - 32; offset += 32) {
            __m256i vec = _mm256_loadu_si256( reinterpret_cast<const __m256i*>(data + offset + REM) );
            __m256i vl = _mm256_unpacklo_epi8(vec, zeroVec);
            __m256i vh = _mm256_unpackhi_epi8(vec, zeroVec);
            
            accum = _mm256_add_epi32(accum, _mm256_madd_epi16(vl, oneVec));
            accum = _mm256_add_epi32(accum, _mm256_madd_epi16(vh, oneVec));
        }

        accum = _mm256_add_epi32(accum, _mm256_srli_si256(accum, 4));
        accum = _mm256_add_epi32(accum, _mm256_srli_si256(accum, 8));

        // When the delimiter is of base 2 (256 == 2^8) you can use bitoperations instead of modulo. 
        // If your compiler is smart enough it will use this method anyway, but just to be sure, its better to just hardcode this logic. 
        return ( _mm256_extract_epi32(accum, 0) + _mm256_extract_epi32(accum, 4) + remainder ) & (256 - 1);
    }


    
} // namespace ctrader::parser

