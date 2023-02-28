#pragma once

#include <iostream>
#include <string> 
#include <utility> // std::move
#include <cmath> // std::pow, std::abs, std::modf
#include <exception>

#include "types/numbers.hpp"
#include "tools/numbers.hpp"
#include "tools/memory.hpp"

namespace {
    using namespace ctrader::types::numbers;
    using namespace ctrader::tools::numbers;

    bool is_whole_part_overflow( float value ){
        float w;
        std::modf(value, &w);
        if (w >= 10'000'000'000.0f) { return true; }
        return false;
    }

}

namespace ctrader::types::decimal {
    using namespace ctrader::types::numbers;

    struct f32 {

        constexpr f32(): whole_part(0), fractional_part(0), fractional_subbase(4) {};
        constexpr f32( i32& a, u32& b, u8& c ): whole_part(a), fractional_part(b), fractional_subbase(c) {};
        constexpr f32( const i32& a, const u32& b, const u8& c ): whole_part(a), fractional_part(b), fractional_subbase(c) {};
        constexpr f32( i32&& a, u32&& b, u8&& c ): whole_part(std::move(a)), fractional_part(std::move(b)), fractional_subbase(std::move(c)) {};
        constexpr f32( const i32&& a, const u32&& b, const u8&& c ): whole_part(std::move(a)), fractional_part(std::move(b)), fractional_subbase(std::move(c)) {};
        f32( float value ) { from_float(value); }
        f32( float value, u8 num_places ) { from_float(value, num_places); }

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

        f32& operator+() { std::abs(whole_part); return *this; }
        f32& operator-() { whole_part *= -1; return *this;  }

        f32& operator+=(i32 rhs){ whole_part += rhs; return *this; }
        f32& operator-=(i32 rhs){ whole_part -= rhs; return *this; }

        // 1.0422 + 15.24332 = 16.28552
        f32& operator+(const f32& rhs) { 
            whole_part += rhs.whole_part;
            fractional_part += rhs.fractional_part; 
            
            // true if lhs < rhs. The higher the fractional_subbase the lower the value of lhs becomes.
            u32 noBaseUnderflowMask = 0 - op::gte(fractional_subbase, rhs.fractional_subbase); 
            fractional_subbase = (rhs.fractional_subbase & noBaseUnderflowMask) + (fractional_subbase & ~noBaseUnderflowMask); 

            return *this;
        }
        f32& operator-(const f32& rhs) {             
            u32 lhsFracState = fractional_part - rhs.fractional_part;
            u32 rhsFracState = rhs.fractional_part - fractional_part;

            u32 noBaseUnderflowMask = 0 - op::gte(fractional_subbase, rhs.fractional_subbase);
            u32 positiveLhsMask = 0 - op::gte(lhsFracState, 0);
            u32 positiveRhsMask = 0 - op::gte(rhsFracState, 0); 

            whole_part -= (rhs.whole_part + (1 & (~positiveLhsMask & ~positiveRhsMask) ) );

            fractional_part = (
                ( (rhsFracState & noBaseUnderflowMask) & positiveRhsMask ) + 
                ( (lhsFracState & ~noBaseUnderflowMask) & positiveLhsMask )
            ); 

            fractional_subbase = (
                ( (rhs.fractional_subbase & noBaseUnderflowMask) & positiveRhsMask ) + 
                ( (fractional_subbase & ~noBaseUnderflowMask) & positiveLhsMask )
            ); 

            return *this;         
        }

        operator float() const {
            using namespace ctrader::tools::numbers;
            float base = 0.0f;
            base += float(whole_part);
            base += float( fractional_part / std::pow(10, digit_count(fractional_part) + fractional_subbase ) );
            return base; 
        }
        operator unsigned() const { return static_cast<u32>(whole_part); }
        operator signed() const { return whole_part; }

        inline void from_cstr(const char* buff, const u32 size) {
            using namespace ctrader::tools::memory;
            using namespace ctrader::tools::numbers;
            u32 pos_d = find<16>( buff, '.' );
            u32 size_frac = size - (pos_d + 1U);

            whole_part = to_num<i32>(buff, pos_d);
            fractional_part = to_num<u32>(buff+pos_d+1, size_frac);
            fractional_subbase = find_end<5>( buff+pos_d+1, '0' );
        };
        std::string to_str() const {
            std::string tmp;
            tmp += std::to_string(whole_part) + "." + std::string(fractional_subbase, '0') + std::to_string(fractional_part);
            return tmp;
        };
        friend std::ostream& operator<<(std::ostream& os, const f32& self){
            os << self.whole_part << "." << std::string(self.fractional_subbase, '0') << self.fractional_part;
            return os;
        };

        void from_float( float value, u8 num_places = 9){
        
            if ( is_whole_part_overflow(value) || (num_places > 9) ){
                throw std::overflow_error("Cannot generate f32 type from value as the integral part or fractional part has more than 9 digits!");
            } 
            
            float w, w2, f, f2 = 1.0f;
            f = std::modf(value, &w);
            f = std::fabs(f);

            whole_part = static_cast<i32>( w );

            fractional_subbase = 0;

            // 0.000042245211 --> 4.2245211
            while(f < 1.0f){ fractional_subbase++; f *= 10.0f; }
            fractional_subbase--;

            // 4.2245211 --> 42245211.0
            while( f2 > 0.0f ){ f2 = std::modf(f, &w2); f *= 10.0f; }

            // 42245211.0 --> 42245211 --> 42{num_places} 
            f2 = std::modf(f, &w2);
            u32 fracValue = static_cast<u32>( w2 ); 
            u8 fracDigitSize = digit_count<u32, u8>(fracValue);
            u8 roundingFactor = fracDigitSize > num_places ? (fracDigitSize - num_places) + 1 : 0;

            fractional_part = static_cast<u32>( w2 / std::pow(10, roundingFactor) );
        }

    private:     
        i32 whole_part; 
        u32 fractional_part;
        u8 fractional_subbase; 
    };


} // ctrader::types::decimal