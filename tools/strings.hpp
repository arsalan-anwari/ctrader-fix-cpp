#pragma once

#include <immintrin.h>
#include <cstring>
#include <numeric>
#include <span>

#include "../types/exec_policy.hpp"
#include "../types/numbers.hpp"
#include "../types/concepts.hpp"

namespace {
    using namespace ctrader;
    
    inline void strcpy_scalar(char* dst, const char* src, size_t size) {
        std::memcpy(dst, src, size);
    }

    template<unsigned NumPermutes = 1>
    inline void strcpy_avx(char* dst, const char* src) {
        for (size_t i = 0; i < NumPermutes; i++) {
            _mm_storeu_si128(
                reinterpret_cast<__m128i*>(dst),
                _mm_load_si128(reinterpret_cast<__m128i const*>(src))
            );
            dst += 16; src += 16;
        }
    }

    template<unsigned NumPermutes = 1>
    inline void strcpy_avx2(char* dst, const char* src) {
        for (size_t i = 0; i < NumPermutes; i++) {
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(dst),
                _mm256_load_si256(reinterpret_cast<__m256i const*>(src))
            );
            dst += 32; src += 32;
        }
    }

    inline u32 ascii_sum_scalar(const char* src, size_t size) {
        return std::accumulate(src, src + size, 0);
    }

    template<unsigned NumPermutes = 1>
    inline u32 ascii_sum_avx(const char* src) {

        const __m128i mask_low = _mm_set1_epi32(0x000000ff);
        __m128i accum = _mm_setzero_si128();

        for (size_t i = 0; i < NumPermutes; i++) {
            const __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));

            const __m128i rshift_8 = _mm_srli_epi32(chunk, 8);
            const __m128i rshift_16 = _mm_srli_epi32(chunk, 16);
            const __m128i rshift_24 = _mm_srli_epi32(chunk, 24);

            const __m128i and_0 = _mm_and_si128(mask_low, chunk);
            const __m128i and_8 = _mm_and_si128(mask_low, rshift_8);
            const __m128i and_16 = _mm_and_si128(mask_low, rshift_16);

            accum = _mm_add_epi32(accum, and_0);
            accum = _mm_add_epi32(accum, and_8);
            accum = _mm_add_epi32(accum, and_16);
            accum = _mm_add_epi32(accum, rshift_24);

            src += 16;
        }

        return static_cast<u32>(_mm_extract_epi32(accum, 0)) +
            static_cast<u32>(_mm_extract_epi32(accum, 1)) +
            static_cast<u32>(_mm_extract_epi32(accum, 2)) +
            static_cast<u32>(_mm_extract_epi32(accum, 3));
    }

    template<unsigned NumPermutes = 1>
    inline u32 ascii_sum_avx2(const char* src) {
        const __m256i zero = _mm256_setzero_si256();
        __m256i accum = zero;
        for (size_t i = 0; i < NumPermutes; i++) {
            const __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
            const __m256i sum = _mm256_sad_epu8(chunk, zero);
            accum = _mm256_add_epi32(accum, sum);
            src += 32;
        }

        return (
            static_cast<u32>(_mm256_extract_epi32(accum, 0)) +
            static_cast<u32>(_mm256_extract_epi32(accum, 1)) +
            static_cast<u32>(_mm256_extract_epi32(accum, 2)) +
            static_cast<u32>(_mm256_extract_epi32(accum, 3))
        );
    }

}

namespace ctrader {

    template<
        size_t Size,
        exec_policy Policy = policy_from_min_size_epi8(Size),
        size_t UnalignableSize = unaligned_size_from_stride_epi8(Size, Policy),
        size_t AlignableSize = Size - UnalignableSize
    > 
    inline void strcpy(char* dst, const char* src) {

        if constexpr (Policy == exec_policy::avx) {

            strcpy_scalar(dst, src, UnalignableSize);
            strcpy_avx<
                permutations_from_stride_epi8(AlignableSize, stride_from_policy_epi8(Policy))
            >(dst + UnalignableSize, src + UnalignableSize);

        }

        else if constexpr (Policy == exec_policy::avx2) {

            // AVX copy is slightly faster than scalar copy, as you can move 16 bytes in one clock cycle.
            if constexpr (UnalignableSize >= 16) {
                strcpy_avx(dst, src);
                strcpy_scalar(dst + 16, src + 16, UnalignableSize - 16);
            }
            else {
                strcpy_scalar(dst, src, UnalignableSize);
            }

            strcpy_avx2<
                permutations_from_stride_epi8(AlignableSize, stride_from_policy_epi8(Policy))
            >(dst + UnalignableSize, src + UnalignableSize);

        }

        else {
            strcpy_scalar(dst, src, Size);
        }
    };

    template<
        size_t Size,
        exec_policy Policy = policy_from_min_size_epi8(Size),
        size_t UnalignableSize = unaligned_size_from_stride_epi8(Size, Policy),
        size_t AlignableSize = Size - UnalignableSize
    >
    inline u32 ascii_sum(const char* src) {
        u32 total = 0;

        if constexpr (Policy == exec_policy::avx) {

            total += ascii_sum_scalar(src, UnalignableSize);
            total += ascii_sum_avx<
                permutations_from_stride_epi8(AlignableSize, stride_from_policy_epi8(Policy))
            >(src + UnalignableSize);

        }

        else if constexpr (Policy == exec_policy::avx2) {

            // scalar sum is faster for size < 32. AVX/AVX2 sum is only faster for continious range > 32
            total += ascii_sum_scalar(src, UnalignableSize);
            total += ascii_sum_avx2<
                permutations_from_stride_epi8(AlignableSize, stride_from_policy_epi8(Policy))
            >(src);
        }

        else {
            total += ascii_sum_scalar(src, Size);
        }

        return total;

    };

}