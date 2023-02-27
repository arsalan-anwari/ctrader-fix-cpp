#pragma once

#include "types/decode.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"
#include "settings.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::decode;
using namespace ctrader::types::symbol;
using namespace ctrader::settings;

struct Decoder{

    u32 decode_any(const char* data);
    template<DECODE_TYPE T> u32 decode(const char* data);

private:

    inline void create_insert_order_inc(const char* chunk, u16 entryIdx, u16 size);
    inline void create_remove_order_inc(const char* chunk, u16 entryIdx, u16 size);

    template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
    void decode_algorithm(const char* data, const u32 data_size, const u32 num_entries);

public:
    u8 index_filter[ DecodeBufferSize ];
    u16 market_indices_begin[ DecodeBufferSize * 2 ] = { 0 };
    u16 market_indices_end[ DecodeBufferSize * 2 ] = { 0 };
    decode_data<DATA_TYPE::MARKET_DATA> market_data[ DecodeBufferSize ];

};



} // ctrader::parser
