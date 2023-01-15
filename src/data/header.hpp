#pragma once

#include "timestamp.hpp"

namespace ctrader::data::header {

    using namespace ctrader::settings;

    namespace internal {

        using namespace ctrader::data;
        using namespace ctrader::tools;

        consteval uint32_t calc_header_size(){
            return (
                2 + broker_settings::BeginString.size() + // 8=FIX.4.4
                3 + 3 + // |9=???
                4 + 1 + // |35=?
                4 + 18 + // |34={0:18}
                timestamp::TimestampSize + 4 + // |52=SendingTimeFormat -> 24-32
                broker_settings::SenderCompID.size() + 4 + // |49=SenderCompID
                broker_settings::TargetCompID.size() + 4 + // |56=TargetCompID
                broker_settings::TargetSubID.size() + 4 + // |57=TargetSubID
                broker_settings::SenderSubID.size() + 4 // |50=SenderSubID
            );
        };

        consteval uint32_t calc_header_remainder_size(uint32_t total){
            uint32_t begin = 
                2 + broker_settings::BeginString.size() + // 8=FIX.4.4
                3 + 3 + // |9=???
                4 + 1 + // |35=?
                4 + 18 + // |34={0:18}
                32; // |52=SendingTimeFormat -> 24-32

            return total - begin;
        }

    } // internal

    constexpr auto HeaderBuffSize = internal::calc_header_size();
    constexpr auto HeaderRemainderSize = internal::calc_header_remainder_size(HeaderBuffSize);

    struct header_t {
        union {
            char raw[ HeaderBuffSize ];
            struct {
                char BeginString[ 2 + broker_settings::BeginString.size() ]; // 8=FIX.4.4
                char BodyLength[3 + 3]; // |9=???
                char MsgType[4 + 1]; // |35=?
                char MsgSeqNum[4 + 18]; // |34={0:18}
                char timestamp_32a[32]; // |52=SendingTimeFormat|49=
                char remainder[ HeaderRemainderSize ];
            } field;
        };
    };

} // ctrader::data::header