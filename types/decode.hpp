#pragma once

#include <string_view>

#include "types/numbers.hpp"
#include "types/decimal.hpp"
#include "types/symbol.hpp"

#include "settings.hpp"

namespace ctrader::types::decode {

    using namespace ctrader::settings;
    using namespace ctrader::types::numbers;
    using namespace ctrader::types::decimal;
    using namespace ctrader::types::symbol;

    enum class UPDATE_ACTION : u32 { NEW, UNKNOWN, DELETE };

    enum class ENTRY_TYPE : u32 { BID, OFFER, UNKNOWN };

    enum class DECODE_TYPE: u32 {
        MARKET_DATA_SNAPSHOT,
        MARKET_DATA_INCREMENTAL
    };

    enum class DATA_TYPE: u32 {
        MARKET_DATA,
        QUOTE_DATA
    };

    template <DATA_TYPE T> struct decode_data {};

    template<>
    struct decode_data<DATA_TYPE::MARKET_DATA> {
        UPDATE_ACTION UpdateAction = UPDATE_ACTION::UNKNOWN;
        ENTRY_TYPE EntryType = ENTRY_TYPE::UNKNOWN;
        SYMBOL Symbol = SYMBOL::UNKNOWN;
        i64 EntryId = -1;
        f32 EntryPrice = f32{};
        i64 EntrySize = -1;
    };
    
}