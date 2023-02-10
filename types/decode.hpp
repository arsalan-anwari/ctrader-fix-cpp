#pragma once

#include <string_view>

#include "types/numbers.hpp"
#include "types/memory.hpp"
#include "types/symbol.hpp"

#include "settings.hpp"

namespace ctrader::types::decode {

    using namespace ctrader::settings;
    using namespace ctrader::types::numbers;
    using namespace ctrader::types::memory;
    using namespace ctrader::types::symbol;

    enum class UPDATE_ACTION : u32 { NEW, UNKNOWN, DELETE };
    UPDATE_ACTION UPDATE_ACTION_LOOKUP[3] = { UPDATE_ACTION::NEW, UPDATE_ACTION::UNKNOWN, UPDATE_ACTION::DELETE };

    enum class ENTRY_TYPE : u32 { BID, OFFER, UNKNOWN };
    ENTRY_TYPE ENTRY_TYPE_LOOKUP[3] = { ENTRY_TYPE::BID, ENTRY_TYPE::OFFER, ENTRY_TYPE::UNKNOWN };

    enum class DECODE_TYPE: u32 {
        MARKET_DATA_SNAPSHOT,
        MARKET_DATA_INCREMENTAL
    };

    enum class DATA_TYPE: u32 {
        MARKET_DATA,
        QUOTE_DATA
    };

    struct decode_metadata { u32 offset = 0; u32 size = 0; };
    struct index_range { u16 begin = 0; u16 end = 0; };

    template <DATA_TYPE T> struct decode_data {};

    template<>
    struct decode_data<DATA_TYPE::MARKET_DATA> {
        UPDATE_ACTION UpdateAction = UPDATE_ACTION::UNKNOWN;
        ENTRY_TYPE EntryType = ENTRY_TYPE::UNKNOWN;
        SYMBOL Symbol = SYMBOL::UNKNOWN;
        i64 EntryId = -1;
        float EntryPrice = -1.0;
        i64 EntrySize = -1;
    };
    

    template<DATA_TYPE T>
    using message_container = simple_buffer_t<decode_data<T>, 128>;

    using market_index_container = sparse_chunk_buffer_2d_t<index_range, 128, 4>;

    using market_index_filter = simple_buffer_t<u8, 128>;



}