#pragma once

#include <stdint.h>

namespace ctrader::types::memory {

    template <typename T, size_t N>
    struct simple_buffer_t{
        const size_t size = N;
        size_t real_size = 0;
        T data[N];
    };

    template <typename T, size_t CHUNK_SIZE, size_t CHUNK_NUM>
    struct sparse_chunk_buffer_2d_t {
        const size_t chunk_size = CHUNK_SIZE;
        T data[CHUNK_SIZE*CHUNK_NUM];
        constexpr T* operator[](size_t i){ return &data[chunk_size*i];  };
        const constexpr T* operator[](size_t i) const { return &data[ chunk_size*i ]; }
    };

}