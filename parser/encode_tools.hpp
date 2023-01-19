#pragma once

#include <chrono>
#include <string>

#include "encode.hpp"
#include "concepts"

namespace ctrader::parser::encode_tools {

using MSG = ctrader::data::message_type::MSG;
using CONN = ctrader::data::message_type::CONN;

namespace internal {
    using namespace ctrader::settings;

    constexpr std::string dummyFieldID = std::string(FieldIDDigitSize, '0');

    template<MSG M, CONN C, typename TIME_PERIOD, typename... FIELD_TYPE>
    requires ctrader::tools::concepts::is_time_duration<TIME_PERIOD>
    static inline __attribute__((always_inline))
    void sawc( const TIME_PERIOD & clock, const TIME_PERIOD & elapsed, FIELD_TYPE... fields ){
        auto now = std::chrono::steady_clock::now;
        auto duration = clock - elapsed;
        auto stop_time = now() + duration;
        while( now() < stop_time ){ ctrader::parser::encode::internal::prepare_message<M, C>(fields...); };
    };

} // internal

template<CONN C, typename TIME_PERIOD = std::chrono::nanoseconds> 
requires ctrader::tools::concepts::is_time_duration<TIME_PERIOD>
struct ClockSync {
    ClockSync(const uint16_t cycle_ns) :
        clock( TIME_PERIOD{ cycle_ns } )
    {};

    template<MSG M>
    inline __attribute__((always_inline))
    void sync_and_warm_cache( const TIME_PERIOD & elapsed ){
        if constexpr ( __COMPILE_TIME_CHECK_MSG_TYPE(M, MD_REQ_SUB_DEPTH) ){
            internal::sawc<M, C, TIME_PERIOD>( clock, elapsed, internal::dummyFieldID.c_str(), 0 ); 
        } else {
            internal::sawc<M, C, TIME_PERIOD>( clock, elapsed ); 
        }


        return;
    };

    const TIME_PERIOD clock;

};


} // ctrader::parser::encode_tools