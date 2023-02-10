#pragma once

#include "types/numbers.hpp"

namespace ctrader::types::memory {
    using namespace ctrader::types::numbers;

    template <typename T, auto N>
    struct simple_buffer_t{
        const std::size_t size = N;
        std::size_t data_len = 0;
        T data[N];
    };

    template <typename T, auto CHUNK_SIZE, auto CHUNK_NUM>
    struct sparse_chunk_buffer_2d_t {
        const std::size_t chunk_size = CHUNK_SIZE;
        T data[CHUNK_SIZE*CHUNK_NUM];
        constexpr T* operator[](const auto i){ return &data[chunk_size*i];  };
        const constexpr T* operator[](const auto i) const { return &data[ chunk_size*i ]; }
    };

}