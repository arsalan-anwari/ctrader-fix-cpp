#pragma once

#include <x86intrin.h>
#include <algorithm>

#include "concepts.hpp"

namespace ctrader::tools::memory {

    using namespace ctrader::tools::concepts;

    template<std::size_t SIZE> requires is_32byte_alignable<SIZE>
    static inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void memcpy_32a(char* dst, const char* src){
        for(std::size_t offset = 0; offset <= SIZE - 32; offset += 32) {
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst+offset),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src+offset) )
            );
        }
    };

    template<std::size_t SIZE, uint8_t REM = SIZE % 32> requires is_minimum_size<SIZE, 32>
    static inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void memcpy_32u(char* dst, const char* src){

        for(uint8_t i=0; i < REM; i++){ dst[i] = src[i]; }

        for(std::size_t offset = 0; offset <= SIZE - 32; offset += 32) {
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst+offset+REM),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src+offset+REM) )
            );
        }
        
    };

    static inline __attribute__((always_inline))
    void memcpy_32(char* dst, const char* src){
        
            _mm256_store_si256(
                reinterpret_cast<__m256i*>(dst),
                _mm256_load_si256( reinterpret_cast<__m256i const*>(src) )
            );

    };

    template <typename T, std::size_t N>
    struct simple_buffer_t{
        std::size_t size = N;
        T data[N];
    };

    template <typename T, std::size_t CHUNK_SIZE, std::size_t CHUNK_NUM>
    struct sparse_chunk_buffer_2d_t {
        std::size_t chunksize = CHUNK_SIZE;
        T data[CHUNK_SIZE*CHUNK_NUM];
        constexpr T* operator[](std::size_t i){ return &data[chunksize*i];  };
        const constexpr T* operator[](std::size_t i) const { return &data[ chunksize*i ]; }
    };

    template<typename T, std::size_t N>
    constexpr simple_buffer_t<T, N> simple_buffer_from_buffer(const T* buff){
        simple_buffer_t<T, N> res;
        std::copy( buff, buff+N, res.data );
        return res;
    };


} // namespace afw

