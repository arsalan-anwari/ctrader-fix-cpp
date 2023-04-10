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

    enum class request : u8 {
        market_data_req,
        new_order,
        undefined
    };

    template<request T>
    struct request_data {};

    template<> struct request_data<request::market_data_req> {
        md_action update_action = md_action::undefined;
        md_type entry_type = md_type::undefined;
        u64 entry_id = 0;
        symbol symbol = symbol::undefined;
        float entry_price = -1.0f;
        u64 entry_size = 0;
    };

    struct range_t {
        u8 min; 
        u8 max;
    };

    constexpr range_t symbol_skip_sizes[] = {
        {0, 0},
        {25, 50}
    };

}
}