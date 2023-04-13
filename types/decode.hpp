#pragma once

#include "numbers.hpp"
#include "symbol.hpp"

namespace ctrader {
namespace decode {

    enum class md_action : u8 {
        insert = 0,
        remove = 2,
        undefined = 0
    };

    enum class md_type : u8 {
        bid = 0,
        offer = 1,
        undefined = 0
    };


    template<response T>
    struct response_data {};

    template<> struct response_data<response::market_data_incremental> {
        md_action update_action = md_action::undefined;
        md_type entry_type = md_type::undefined;
        u64 entry_id = 0;
        symbol symbol = symbol::undefined;
        float entry_price = -1.0f;
        u64 entry_size = 0;
    };

    struct range_t {
        u8 begin; 
        u8 end;
    };

    constexpr range_t SYMBOL_SKIP_SIZES[] = {
        {0, 0},
        {25, 50}
    };

}
}