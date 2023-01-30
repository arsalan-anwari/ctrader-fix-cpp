#pragma once

#include <stdint.h>
#include <string_view>

namespace ctrader::types::symbol {

    enum class SYMBOL : uint64_t {
        UNKNOWN = 0UL,
        EUR_USD= 1UL
    };

    namespace internal {

        constexpr std::string_view symbol_data[] = {
            "0", "1"
        };        
        
    } // namespace internal

    namespace SymbolData{
         inline __attribute__((always_inline)) 
        auto get_data(const SYMBOL& symbol) -> const char* { return internal::symbol_data[static_cast<uint64_t>(symbol)].data(); };
        
         inline __attribute__((always_inline)) 
        auto get_size(const SYMBOL& symbol) -> std::size_t { return internal::symbol_data[static_cast<uint64_t>(symbol)].size(); };
    };

}