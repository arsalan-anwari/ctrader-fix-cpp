#pragma once

#include "types/decode.hpp"
#include "types/symbol.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::types::symbol;

struct Decoder{

    Decoder();
    template<DECODE_TYPE D = DECODE_TYPE::UNKNOWN> u16 decode(const char* data);
    template<DATA_TYPE T> DATA_BUFF<T> const& get_decode_data() const;

private:

    template<DECODE_TYPE D, UPDATE_ACTION U>
    inline __attribute__((always_inline)) 
    void insert_entry(const char* chunk, u16 entryIdx, u16 size);

    template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
    void decode_algorithm(const char* data, const u32 data_size, const u16 num_entries);

private:
    u16 index_filter[ DecodeBufferSize ] = { 0 };
    u32 market_indices_begin[ DecodeBufferSize * 2 ] = { 0 };
    u32 market_indices_end[ DecodeBufferSize * 2 ] = { 0 };

    DATA_BUFF<DATA_TYPE::MARKET_DATA> market_data;
    DATA_BUFF<DATA_TYPE::QUOTE_DATA> quote_data;

};



} // ctrader::parser
