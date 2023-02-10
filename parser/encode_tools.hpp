#pragma once

#include <chrono>

#include "encode.hpp"
#include "concepts"

namespace ctrader::parser::encode_tools {

using namespace ctrader::types::encode;
using namespace ctrader::tools::concepts;

template<CONN_TYPE C, typename TIME_PERIOD = std::chrono::nanoseconds> 
requires is_time_duration<TIME_PERIOD>
struct ClockSync {
    ClockSync(const u16 cycle_ns) :
        clock( TIME_PERIOD{ cycle_ns } )
    {};

    template<ENCODE_TYPE M>
    inline __attribute__((always_inline))
    void sync_and_warm_cache( const TIME_PERIOD & elapsed ){
        using namespace ctrader::parser::encode;

        auto now = std::chrono::steady_clock::now;
        auto duration = clock - elapsed;
        auto stop_time = now() + duration;
        while( now() < stop_time ){ internal::prepare_message<M, C>(0); };
    };

    const TIME_PERIOD clock;

};


} // ctrader::parser::encode_tools