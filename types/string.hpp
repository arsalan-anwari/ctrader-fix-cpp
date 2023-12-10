#pragma once

#include <concepts>

namespace ctrader {

template<usize size>
struct cv_string {
    char data[size];
    usize length = size;
};


}