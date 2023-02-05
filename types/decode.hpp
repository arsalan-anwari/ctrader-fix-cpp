#pragma once

#include <stdint.h>
#include <string_view>

#include "types/memory.hpp"
#include "types/symbol.hpp"

#include "settings.hpp"

namespace ctrader::types::decode {

    using namespace ctrader::settings;
    using namespace ctrader::types::memory;
    using namespace ctrader::types::symbol;

    enum class UPDATE_ACTION : uint8_t { NEW, UNKNOWN, DELETE };
    UPDATE_ACTION UPDATE_ACTION_LOOKUP[3] = { UPDATE_ACTION::NEW, UPDATE_ACTION::UNKNOWN, UPDATE_ACTION::DELETE };

    enum class ENTRY_TYPE : uint8_t { BID, OFFER, UNKNOWN };
    ENTRY_TYPE ENTRY_TYPE_LOOKUP[3] = { ENTRY_TYPE::BID, ENTRY_TYPE::OFFER, ENTRY_TYPE::UNKNOWN };

    enum class DECODE_TYPE: uint8_t {
        MARKET_DATA_SNAPSHOT,
        MARKET_DATA_INCREMENTAL
    };

    enum class DATA_TYPE: uint8_t {
        MARKET_DATA,
        QUOTE_DATA
    };

    struct decode_metadata { uint16_t offset = 0; uint16_t size = 0; };
    struct index_range { int16_t begin = -1; int16_t end = -1; };

    template <DATA_TYPE T> struct decode_data {};

    template<>
    struct decode_data<DATA_TYPE::MARKET_DATA> {
        UPDATE_ACTION UpdateAction = UPDATE_ACTION::UNKNOWN;
        ENTRY_TYPE EntryType = ENTRY_TYPE::UNKNOWN;
        SYMBOL Symbol = SYMBOL::UNKNOWN;
        int64_t EntryId = -1;
        float EntryPrice = -1.0;
        int64_t EntrySize = -1;
    };
    

    template<DATA_TYPE T>
    using message_container = simple_buffer_t<decode_data<T>, 256>;

    using market_index_container = sparse_chunk_buffer_2d_t<index_range, 256, 4>;

    using market_index_filter = simple_buffer_t<uint8_t, 256>;



}