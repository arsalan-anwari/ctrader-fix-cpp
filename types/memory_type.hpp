#pragma once

#include <stdint.h>

namespace ctrader::types::memory_type {

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

}