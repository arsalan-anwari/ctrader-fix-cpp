#pragma once

#include <x86intrin.h>

#include "types/memory.hpp"
#include "types/numbers.hpp"
#include "types/execution.hpp"
#include "types/concepts.hpp"

namespace {
    using namespace ctrader::types::numbers;
    using namespace ctrader::types::execution;

    consteval u32 min_permutes( const u32 size, const u8 chunk_size ){
        i32 remainder = size;
        u32 len = 0;
        while( remainder > 0 ){
            remainder -= chunk_size;
            len++;
        }

        return len;
    }

    consteval EXEC_TYPE get_exec_type_epi8( const u32 stride ){
        if (stride <= 16) { return EXEC_TYPE::AVX; }
        if (stride <= 32) { return EXEC_TYPE::AVX2; }
        if (stride <= 64) { return EXEC_TYPE::AVX512; }
        return EXEC_TYPE::SCALAR;
    }

    template<EXEC_TYPE T>
    inline u32 _find( const char* buff, const char c );

    template<EXEC_TYPE T>
    inline u32 _find_end( const char* buff, const char c );

    template<> u32 _find<EXEC_TYPE::AVX>( const char* buff, const char c ){
        __m128i haystack_vec = _mm_load_si128( reinterpret_cast<const __m128i*>(buff) );
        const __m128i needle_vec = _mm_set1_epi8(c);
        __m128i result_vec = _mm_cmpeq_epi8(haystack_vec, needle_vec);

        i32 result_mask = _mm_movemask_epi8(result_vec);
        i32 result_mask_is_invalid = op::eq(result_mask, 0);
        u32 found_pos = __builtin_ctz(result_mask + result_mask_is_invalid);
        return found_pos;
    } 

    template<> u32 _find<EXEC_TYPE::AVX2>( const char* buff, const char c ){
        __m256i haystack_vec = _mm256_load_si256( reinterpret_cast<const __m256i*>(buff) );
        const __m256i needle_vec = _mm256_set1_epi8(c);
        __m256i result_vec = _mm256_cmpeq_epi8(haystack_vec, needle_vec);

        i32 result_mask = _mm256_movemask_epi8(result_vec);
        i32 result_mask_is_invalid = op::eq(result_mask, 0);
        u32 found_pos = __builtin_ctz(result_mask + result_mask_is_invalid);
        return found_pos;
    } 

    template<> u32 _find_end<EXEC_TYPE::AVX>( const char* buff, const char c ){
        __m128i haystack_vec = _mm_load_si128( reinterpret_cast<const __m128i*>(buff) );
        const __m128i needle_vec = _mm_set1_epi8(c);
        __m128i result_vec = _mm_cmpeq_epi8(haystack_vec, needle_vec);

        i32 result_mask = ~(_mm_movemask_epi8(result_vec));
        i32 result_mask_is_invalid = op::eq(result_mask, 0);
        u32 trailing_size = __builtin_ctz(result_mask + result_mask_is_invalid);

        return trailing_size;
    } 

    template<> u32 _find_end<EXEC_TYPE::AVX2>( const char* buff, const char c ){
        __m256i haystack_vec = _mm256_load_si256( reinterpret_cast<const __m256i*>(buff) );
        const __m256i needle_vec = _mm256_set1_epi8(c);
        __m256i result_vec = _mm256_cmpeq_epi8(haystack_vec, needle_vec);

        i32 result_mask = ~(_mm256_movemask_epi8(result_vec));
        i32 result_mask_is_invalid = op::eq(result_mask, 0);
        u32 trailing_size = __builtin_ctz(result_mask + result_mask_is_invalid);

        return trailing_size;
    } 

}

namespace ctrader::tools::memory {

    using namespace ctrader::types::concepts;
    using namespace ctrader::types::memory;
    using namespace ctrader::types::numbers;
    using namespace ctrader::types::execution;

    template<std::size_t SIZE> requires is_32byte_alignable<SIZE>
    inline void memcpy_32a(char* dst, const char* src){
        for(std::size_t offset = 0; offset <= SIZE - 32; offset += 32) {
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst+offset),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src+offset) )
            );
        }
    };

    template<std::size_t SIZE, u8 REM = SIZE % 32> requires is_minimum_size<SIZE, 32>
    inline void memcpy_32u(char* dst, const char* src){

        for(u8 i=0; i < REM; i++){ dst[i] = src[i]; }

        for(std::size_t offset = 0; offset <= SIZE - 32; offset += 32) {
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst+offset+REM),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src+offset+REM) )
            );
        }
        
    };

    inline void memcpy_32(char* dst, const char* src){
        
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src) )
            );

    };

    template<u32 MAX_SEEK_SIZE, EXEC_TYPE T = get_exec_type_epi8(MAX_SEEK_SIZE)>
    inline u32 find( const char* buff, const char c ) { return _find<T>(buff, c); }

    template<u32 MAX_SEEK_SIZE, EXEC_TYPE T = get_exec_type_epi8(MAX_SEEK_SIZE)>
    inline u32 find_end( const char* buff, const char c ){ return _find_end<T>(buff, c); }

    // Optimization of EPSMA-1 algorithm which can find patterns in the entire 32 byte chunk instead of only the first 8 bytes. 
    // This does force you to perform (max_index / 8) + 1 permutations. 
    template <u32 MAX_SEEK_SIZE, u32 PERMUTES = min_permutes(MAX_SEEK_SIZE, 8)>
    inline u32 find_pattern_full(const char* chunk, const char* pattern){
        __m256i chunk_vec = _mm256_loadu_si256( reinterpret_cast<const __m256i*>(chunk) );
        __m256i pattern_vec = _mm256_loadu_si256( reinterpret_cast<const __m256i*>(pattern) );
        __m256i sad_vec, found_matches;

        const __m256i zero_mask = _mm256_setzero_si256();
        const __m256i permute = _mm256_setr_epi32(2, 3, 4, 5, 6, 7, 1, 0);

        i32 match_mask = 0, match_mask_is_valid = 0;
        u32 idx = 0, offset = 0, match_mask_is_valid_mask = 0, trail_count = 0;

        for(u32 i=0; i<PERMUTES; i++){
            sad_vec = _mm256_mpsadbw_epu8(chunk_vec, pattern_vec, 0);
            found_matches = _mm256_cmpeq_epi16(sad_vec, zero_mask);

            match_mask = _mm256_movemask_epi8(found_matches);
            match_mask_is_valid = op::gte(match_mask, 1);
            match_mask_is_valid_mask = 0 - match_mask_is_valid;

            trail_count = __builtin_ctz(match_mask + (match_mask_is_valid ^ 1) ) >> 1;
            idx += (offset + trail_count + 1) & match_mask_is_valid_mask;
            
            offset += 8;
            chunk_vec = _mm256_permutevar8x32_epi32(chunk_vec, permute);
        }

        return idx;
    };

    // Based on EPSMA-1 algorithm but in this version you only need to perform a single permutation for the first 8 bytes vs `len_of_pattern` permutations
    // - If you want to increase the probability that patterns are detected correctly you can perform an additional `_mm256_mpsadbw_epu8()` 
    //   offset by `len_of_pattern` and substract `len_of_pattern` from the found index. This second permutation is not nessecary however as index are always in range 0 - 8. 
    //      * This is because in the future I will port my probability distribution function which will calculate correct skip_sizes (see 'decode_algorithm()')
    //        based on previously computed indices. This will ensure that the pattern will majority of the time be within the first 8 - 'len_of_pattern' when searching for it
    //        meaning only a single permutation is needed not 'len_of_pattern' permutations. 
    inline u32 find_pattern_begin(const char* chunk, const char* pattern){
        __m256i chunk_vec = _mm256_loadu_si256( reinterpret_cast<const __m256i*>(chunk) );
        __m256i pattern_vec = _mm256_loadu_si256( reinterpret_cast<const __m256i*>(pattern) );
        const __m256i zero_mask = _mm256_setzero_si256();

        __m256i sad_vec = _mm256_mpsadbw_epu8(chunk_vec, pattern_vec, 0);
        __m256i found_matches = _mm256_cmpeq_epi16(sad_vec, zero_mask);

        i32 match_mask = _mm256_movemask_epi8(found_matches);
        i32 match_mask_is_invalid = op::lte(match_mask, 0);

        u32 trail_count = __builtin_ctz(match_mask + match_mask_is_invalid) >> 1;
        u32 idx = trail_count + (match_mask_is_invalid ^ 1);

        return idx;
    };


} // namespace afw

