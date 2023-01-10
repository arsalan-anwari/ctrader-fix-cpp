#pragma once

namespace ctrader::tools::concepts {

    template<std::size_t SIZE>
    concept is_32byte_alignable = SIZE % 32 == 0;

    template<std::size_t SIZE, std::size_t MIN>
    concept is_minimum_size = SIZE >= MIN;

} //ctrader::tools::concepts