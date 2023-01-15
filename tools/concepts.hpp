#pragma once

#include <stdint.h>
#include <cstddef>

namespace ctrader::tools::concepts {

    template<std::size_t SIZE>
    concept is_32byte_alignable = SIZE % 32 == 0;

    template<std::size_t SIZE, std::size_t MIN>
    concept is_minimum_size = SIZE >= MIN;

    template<std::size_t VAL>
    concept is_power_of_2 = (VAL > 0) && !(VAL & (VAL - 1));

} //ctrader::tools::concepts