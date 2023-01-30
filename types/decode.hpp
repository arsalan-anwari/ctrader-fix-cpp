#pragma once

#include <stdint.h>
#include <string_view>

#include "settings.hpp"

namespace ctrader::types::decode {

    using namespace ctrader::settings;

    enum class UPDATE_ACTION : uint8_t { NEW, UNKNOWN, DELETE };
    enum class ENTRY_TYPE : uint8_t { BID, OFFER, UNKNOWN };

    enum class DECODE_TYPE: uint8_t {
        MARKET_DATA_SNAPSHOT,
        MARKET_DATA_INCREMENTAL
    };

    #define __DECODE_GEN_PATTERN(pattern) __SETTINGS_SOH #pattern

    const char* const DECODE_TYPE_PATTERN_LOOKUP[] = {
            __DECODE_GEN_PATTERN(269),
            __DECODE_GEN_PATTERN(279)
    };

    struct decode_metadata { uint16_t offset; uint16_t size; };

    template <DECODE_TYPE T> struct decode_data {};

    template<>
    struct decode_data<DECODE_TYPE::MARKET_DATA_SNAPSHOT> {
        UPDATE_ACTION UpdateAction = UPDATE_ACTION::UNKNOWN;
        int64_t EntryId = -1;
        int64_t SymbolId = -1;
    };

    template<>
    struct decode_data<DECODE_TYPE::MARKET_DATA_INCREMENTAL> {
        UPDATE_ACTION UpdateAction = UPDATE_ACTION::UNKNOWN;
        ENTRY_TYPE EntryType = ENTRY_TYPE::UNKNOWN;
        int64_t EntryId = -1;
        int64_t SymbolId = -1;
        float EntryPrice = -1.0;
        int64_t EntrySize = -1;
    };
    

}