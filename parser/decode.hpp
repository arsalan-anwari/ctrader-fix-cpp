#pragma once

#include "types/decode.hpp"
#include "types/symbol.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::types::symbol;

struct Decoder{

    template<DECODE_TYPE D = DECODE_TYPE::UNKNOWN> u32 decode(const char* data);

private:

    template<DECODE_TYPE D, UPDATE_ACTION U>
    inline __attribute__((always_inline)) 
    void insert_entry(const char* chunk, u8 entryIdx, u8 size);

    template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
    void decode_algorithm(const char* data, const u32 data_size, const u32 num_entries);

public:
    u8 index_filter[ DecodeBufferSize ] = { 0 };
    u16 market_indices_begin[ DecodeBufferSize * 2 ] = { 0 };
    u16 market_indices_end[ DecodeBufferSize * 2 ] = { 0 };
    decode_data<DATA_TYPE::MARKET_DATA> market_data[ DecodeBufferSize ];

};



} // ctrader::parser
