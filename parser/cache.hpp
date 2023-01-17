#pragma once

#include "encode.hpp"

namespace ctrader::parser::cache{

using namespace ctrader::parser::encode;

template<MSG_TYPE T, typename... FIELD_TYPE>
static inline __attribute__((always_inline))
void clock_sync_and_warm_cache(const std::chrono::nanoseconds& clock, const std::chrono::nanoseconds& tp_elapsed, FIELD_TYPE... fields){ 
    auto now = std::chrono::steady_clock::now;
    auto duration = clock - tp_elapsed;
    auto stop_time = now() + duration;
    while( now() < stop_time ){ internal::msg_seq_num--; prepare_message<T>(fields...); };
};

}