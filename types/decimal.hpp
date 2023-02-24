#pragma once

#include <iostream>
#include <string> 
#include <utility> // std::move
#include <cmath> // std::pow

#include "types/numbers.hpp"
#include "tools/numbers.hpp"
#include "tools/memory.hpp"

namespace ctrader::types::decimal {
    using namespace ctrader::types::numbers;

    struct f32 {

        f32(): whole_part(0), fractional_part(0), fractional_subbase(4) {};
        f32( u32& a, u32& b, u8& c ): whole_part(a), fractional_part(b), fractional_subbase(c) {};
        f32( const u32& a, const u32& b, const u8& c ): whole_part(a), fractional_part(b), fractional_subbase(c) {};
        f32( u32&& a, u32&& b, u8&& c ): whole_part(std::move(a)), fractional_part(std::move(b)), fractional_subbase(std::move(c)) {};
        f32( const u32&& a, const u32&& b, const u8&& c ): whole_part(std::move(a)), fractional_part(std::move(b)), fractional_subbase(std::move(c)) {};

        f32& operator=(f32&& other) noexcept{
            if (this == &other) { return *this; };
            whole_part = std::move(other.whole_part);
            fractional_part = std::move(other.fractional_part);
            fractional_subbase = std::move(other.fractional_subbase);
            return *this;
        };

        f32& operator=(const f32&& other) noexcept{
            if (this == &other) { return *this; };
            whole_part = std::move(other.whole_part);
            fractional_part = std::move(other.fractional_part);
            fractional_subbase = std::move(other.fractional_subbase);
            return *this;
        };

        friend std::ostream& operator<<(std::ostream& os, const f32& self){
            os << self.whole_part << "." << std::string(self.fractional_subbase, '0') << self.fractional_part;
            return os;
        };

        operator float() const {
            using namespace ctrader::tools::numbers;
            float base = 0.0f;
            base += float(whole_part);
            base += float( fractional_part / std::pow(10, digit_count(fractional_part) + fractional_subbase ) );
            return base; 
        }

        inline void from_cstr(const char* buff, const u32 size) noexcept {
            using namespace ctrader::tools::memory;
            using namespace ctrader::tools::numbers;
            u32 pos_d = find<6>( buff, '.' );
            u32 size_frac = size - (pos_d + 1U);

            whole_part = to_num<u32>(buff, pos_d);
            fractional_part = to_num<u32>(buff+pos_d+1, size_frac);
            fractional_subbase = find_end<5>( buff+pos_d+1, '0' );
        };

        std::string to_str() const {
            std::string tmp;
            tmp += std::to_string(whole_part) + "." + std::string(fractional_subbase, '0') + std::to_string(fractional_part);
            return tmp;
        };

    private:     
        u32 whole_part; 
        u32 fractional_part;
        u8 fractional_subbase; 
    };


} // ctrader::types::decimal