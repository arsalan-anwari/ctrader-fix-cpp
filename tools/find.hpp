#pragma once

#include <immintrin.h>
#include <span>
#include <string_view>
#include <bit>
#include <bitset>

#include "bitwise.hpp"
#include "../types/exec_policy.hpp"
#include "../types/numbers.hpp"
#include "../types/concepts.hpp"

// find_impl
namespace {

    using namespace ctrader;

    inline i32 find_pattern_scalar(std::string_view chunk, std::string_view pattern){
        return static_cast<i32>(chunk.find(pattern));
    }
    
    template<unsigned NumPermutes>
    inline i32 find_pattern_avx2(const char* chunk, const char* pattern) {

        __m256i chunk_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(chunk));
        __m256i pattern_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pattern));
        __m256i sad_vec, found_matches;

        const __m256i zero_mask = _mm256_setzero_si256();
        const __m256i permute = _mm256_setr_epi32(2, 3, 4, 5, 6, 7, 1, 0);

        u32 offset = 0;
        i32 idx = -1;
        i32 pattern_found = 0;
        
        // search |268
        // |262= b3j2 b26| 268= 25|2 79=0 |269 =1|2
        // b26| 268= 25|2 79=0 |269 =1|2 |262= b3j2
        for (unsigned i = 0; i < NumPermutes; i++) {
            sad_vec = _mm256_mpsadbw_epu8(chunk_vec, pattern_vec, 0);
            found_matches = _mm256_cmpeq_epi16(sad_vec, zero_mask);

            u32 match_mask = _mm256_movemask_epi8(found_matches);
            i32 match_mask_is_valid = bitwise::gte(match_mask, 1);
            u32 match_mask_is_valid_mask = 0 - match_mask_is_valid;
            u32 trail_count = std::countr_zero(match_mask) & match_mask_is_valid_mask;
            idx += ((offset + (trail_count/2)) & match_mask_is_valid_mask);
            pattern_found += match_mask_is_valid;

            offset += 8;
            chunk_vec = _mm256_permutevar8x32_epi32(chunk_vec, permute);
        }

        return idx + bitwise::ne(pattern_found, 0);

    }

    template<> i32 find_pattern_avx2<1U>(const char* chunk, const char* pattern) {
        const __m256i chunk_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(chunk));
        const __m256i pattern_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pattern));
        const __m256i zero_mask = _mm256_setzero_si256();

        const __m256i sad_vec = _mm256_mpsadbw_epu8(chunk_vec, pattern_vec, 0);
        const __m256i found_matches = _mm256_cmpeq_epi16(sad_vec, zero_mask);

        u32 match_mask = _mm256_movemask_epi8(found_matches);
        i32 match_mask_is_valid = bitwise::gte(match_mask, 1); // 1 if mm <= 0; 0 if mm > 0
        u32 match_mask_is_valid_mask = 0 - match_mask_is_valid; // 0xFFFF if mm <= 0; 0x0000 if mm > 0

        //i32 trail_count = std::countr_zero(match_mask << 1) & match_mask_is_valid_mask; // {0...31} or -1 
        
        u32 trail_count = std::countr_zero(match_mask) & match_mask_is_valid_mask;
        i32 idx = (trail_count / 2) & match_mask_is_valid_mask;
        
        return idx - (match_mask_is_valid ^ 1);
    }

    inline i32 find_needle_scalar(std::string_view chunk, const char needle) {
        return static_cast<i32>(chunk.find(needle));
    }

    template<unsigned NumPermutes>
    inline i32 find_needle_avx(const char* chunk, const char needle) {
        const __m128i needle_vec = _mm_set1_epi8(needle);
        i32 trail_count = -1;
        i32 needle_not_already_found = 0;

        for (unsigned offset = 0U; offset < (NumPermutes * 16U); offset += 16U) {
            __m128i haystack_vec = _mm_load_si128(reinterpret_cast<const __m128i*>(chunk+offset));
            __m128i result_vec = _mm_cmpeq_epi8(haystack_vec, needle_vec);
            u32 match_mask = _mm_movemask_epi8(result_vec);

            // (1, 0xFF) if mm != 0; (0, 0x00) if mm == 0
            i32 needle_found = bitwise::ne(match_mask, 0);
            const u32 needle_found_mask = 0 - needle_found;
            ;
            // (1, 0xFF) if nnaf == 0; (0, 0x00) if nnaf > 0
            needle_not_already_found += needle_found;
            const u32 needle_not_already_found_mask = 0 - bitwise::eq(needle_not_already_found, 0);

            // {0...15} or -1 
            trail_count += (offset + std::countr_zero(match_mask << 1))
                & (needle_found_mask & needle_not_already_found_mask);
        }
       
        return trail_count;
    }

    template<unsigned NumPermutes>
    inline i32 find_needle_avx2(const char* chunk, const char needle) {
        const __m256i needle_vec = _mm256_set1_epi8(needle);
        i32 trail_count = -1; 
        i32 needle_not_already_found = 0;

        for (unsigned offset = 0U; offset < (NumPermutes * 32U); offset += 32U) {
            __m256i haystack_vec = _mm256_load_si256(reinterpret_cast<const __m256i*>(chunk + offset));
            __m256i result_vec = _mm256_cmpeq_epi8(haystack_vec, needle_vec);
            u32 match_mask = _mm256_movemask_epi8(result_vec);

            // (1, 0xFF) if mm != 0; (0, 0x00) if mm == 0
            i32 needle_found = bitwise::ne(match_mask, 0);
            const u32 needle_found_mask = 0 - needle_found; 
;            
            // (1, 0xFF) if nnaf == 0; (0, 0x00) if nnaf > 0
            needle_not_already_found += needle_found;
            const u32 needle_not_already_found_mask = 0 - bitwise::eq(needle_not_already_found, 0);

            // {0...31} or -1 
            trail_count += (offset + std::countr_zero(match_mask << 1)) 
                & (needle_found_mask & needle_not_already_found_mask);
        }

        return trail_count;
    }

    template<> i32 find_needle_avx<1U>(const char* chunk, const char needle) {
        const __m128i needle_vec = _mm_set1_epi8(needle);
        const __m128i haystack_vec = _mm_load_si128(reinterpret_cast<const __m128i*>(chunk));
        const __m128i result_vec = _mm_cmpeq_epi8(haystack_vec, needle_vec);

        u32 match_mask = _mm_movemask_epi8(result_vec);

        // 0xFF if mm != 0; 0x00 if mm == 0
        i32 needle_found = bitwise::ne(match_mask, 0);
        const u32 needle_found_mask = 0 - needle_found;

        // {0...15} or -1 
        return (std::countr_zero(match_mask) & needle_found_mask) - (needle_found ^ 1);
    }

    template<> i32 find_needle_avx2<1U>(const char* chunk, const char needle) {
        const __m256i needle_vec = _mm256_set1_epi8(needle);
        const __m256i haystack_vec = _mm256_load_si256(reinterpret_cast<const __m256i*>(chunk));
        const __m256i result_vec = _mm256_cmpeq_epi8(haystack_vec, needle_vec);

        u32 match_mask = _mm256_movemask_epi8(result_vec);

        // 0xFF if mm != 0; 0x00 if mm == 0
        i32 needle_found = bitwise::ne(match_mask, 0);
        const u32 needle_found_mask = 0 - needle_found;

        // {0...31} or -1 
        return (std::countr_zero(match_mask) & needle_found_mask) - (needle_found ^ 1);
    }

}

// find_end_impl
namespace {
    using namespace ctrader;

    inline i32 find_end_needle_scalar(std::string_view chunk, const char needle) {
        return static_cast<i32>(chunk.rfind(needle));
    }

    inline i32 find_end_needle_avx(const char* chunk, const char needle) {
        const __m128i needle_vec = _mm_set1_epi8(needle);
        const __m128i haystack_vec = _mm_load_si128(reinterpret_cast<const __m128i*>(chunk));
        const __m128i result_vec = _mm_cmpeq_epi8(haystack_vec, needle_vec);

        u32 match_mask = ~(_mm_movemask_epi8(result_vec));

        // 0xFF if mm != 0; 0x00 if mm == 0
        i32 needle_found = bitwise::ne(match_mask, 0);
        const u32 needle_found_mask = 0 - needle_found;

        // {0...15} or -1 
        return (std::countr_zero(match_mask) & needle_found_mask) - (needle_found ^ 1);
    }

}

namespace ctrader{

    template<
        unsigned Size,
        unsigned PatternSize,
        unsigned NumPermutes = 0,
        exec_policy Policy = policy_from_max_size_epi8(Size)
    >
    inline i32 find(const char* text, const char* pattern) {
        if constexpr (Policy == exec_policy::scalar) {
            return find_pattern_scalar(std::string_view(text, Size), std::string_view(pattern, PatternSize));
        }

        if constexpr (Policy == exec_policy::avx2) {
            if constexpr (NumPermutes > 0) {
                return find_pattern_avx2<NumPermutes>(text, pattern);
            }
            else {
                return find_pattern_avx2<permutations_from_stride_epi8(Size, 8)>(text, pattern);
            }
            
        }

        return -1;
    }

    template<
        unsigned Size,
        exec_policy Policy = policy_from_max_size_epi8(Size)
    >
    inline i32 find(const char* text, const char needle) {
        if constexpr (Policy == exec_policy::scalar) {
            return find_needle_scalar(std::string_view(text, Size), needle);
        }

        if constexpr (Policy == exec_policy::avx) {
            return find_needle_avx<permutations_from_stride_epi8(Size, 16)>(text, needle);
        }

        if constexpr (Policy == exec_policy::avx2) {
            return find_needle_avx2<permutations_from_stride_epi8(Size, 32)>(text, needle);
        }

        return -1;
    };


    template<
        unsigned Size,
        exec_policy Policy = policy_from_max_size_epi8(Size)
    >
    inline i32 find_end(const char* text, const char needle) {
        if constexpr (Policy == exec_policy::scalar) {
            return find_end_needle_scalar(std::string_view(text, Size), needle);
        }

        if constexpr (Policy == exec_policy::avx) {
            return find_end_needle_avx(text, needle);
        }

        //if constexpr (Policy == exec_policy::avx2) {
        //    return find_needle_avx2<permutations_from_stride_epi8(Size, 32)>(text, needle);
        //}

        return -1;
    };


}