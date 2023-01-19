#pragma once

#include <chrono>

#include "encode.hpp"
#include "concepts"

namespace ctrader::parser::encode_tools {

namespace internal {

    using MSG = ctrader::data::message_type::MSG;
    using CONN = ctrader::data::message_type::CONN;

} // internal

template<internal::CONN C, typename TIME_PERIOD = std::chrono::nanoseconds> 
requires ctrader::tools::concepts::is_time_duration<TIME_PERIOD>
struct ClockSync {
    ClockSync(const uint16_t cycle_ns) :
        clock( TIME_PERIOD{ cycle_ns } )
    {};

    template<internal::MSG M>
    inline __attribute__((always_inline))
    void sync_and_warm_cache( TIME_PERIOD elapsed ){
        auto now = std::chrono::steady_clock::now;
        auto duration = clock - elapsed;
        auto stop_time = now() + duration;
        while( now() < stop_time ){ ctrader::parser::encode::internal::prepare_message<M, C>(); };
    };

    const TIME_PERIOD clock;

};

} // ctrader::parser::encode_tools