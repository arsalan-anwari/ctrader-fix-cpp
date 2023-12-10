#pragma once

#include <iostream> // std::ostream
#include <utility> // std::move
#include <cmath> // std::pow, std::abs

#include "numbers.hpp"
#include "../tools/bitwise.hpp"
#include "../tools/math.hpp"

namespace ctrader {

    struct price {

        price() : 
            whole_part(-1), 
            fractional_part(0U), 
            fractional_subbase(0U) 
        {}

        template<typename T>
        price(T&& other) :
            whole_part(other.whole_part),
            fractional_part(other.fractional_part),
            fractional_subbase(other.fractional_subbase)
        {}

        price(i32&& a, u32&& b, u8&& c) : 
            whole_part(a),
            fractional_part(b),
            fractional_subbase(c)
        {}

        price& operator=(price other) noexcept {
            std::swap(whole_part, other.whole_part);
            std::swap(fractional_part, other.fractional_part);
            std::swap(fractional_subbase, other.fractional_subbase);
            return *this;
        }

        price& operator+() { std::abs(whole_part); return *this; }
        price& operator-() { whole_part *= -1; return *this; }

        friend bool operator<(const price& lhs, const price& rhs) {
            return (
                (lhs.whole_part <= rhs.whole_part) &&
                (lhs.fractional_subbase >= rhs.fractional_subbase) &&
                (lhs.fractional_part <= rhs.fractional_part)
            );
        }

        friend bool operator==(const price& lhs, const price& rhs) {
            return (
                (lhs.whole_part == rhs.whole_part) &&
                (lhs.fractional_subbase == rhs.fractional_subbase) &&
                (lhs.fractional_part == rhs.fractional_part)
            );
        }

        auto operator<=>(const price&) const = default;

        operator float() const {
            float base = 0.0f;
            base += static_cast<float>(whole_part);
            base += static_cast<float>(fractional_part / std::pow(10, digit_count(fractional_part) + fractional_subbase));
            return base;
        }
        operator unsigned() const { return static_cast<u32>(whole_part); }
        operator signed() const { return whole_part; }

        friend std::ostream& operator<<(std::ostream& os, const price& self) {
            os << self.whole_part << "." << std::string(self.fractional_subbase, '0') << self.fractional_part;
            return os;
        };

    private:
        i32 whole_part;
        u32 fractional_part;
        u8 fractional_subbase;
    };

}