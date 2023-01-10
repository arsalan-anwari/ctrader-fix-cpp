#pragma once

#include "metadata.hpp"
#include "timestamp.hpp"

namespace ctrader::data::header {

    namespace internal {

        using namespace ctrader::data;
        using namespace ctrader::settings;
        using namespace ctrader::tools;

        consteval int32_t calc_header_size(){
            return (
                metadata::MetadataSize + // {?*n}8=FIX.4.4|9=???|35=?|34=? -> 32+
                timestamp::TimestampSize + 4 + // |52=SendingTimeFormat -> 24-32
                broker_settings::SenderCompID.size() + 4 + // |49=SenderCompID
                broker_settings::TargetCompID.size() + 4 + // |56=TargetCompID
                broker_settings::TargetSubID.size() + 4 + // |57=TargetSubID
                broker_settings::SenderSubID.size() + 4 // |50=SenderSubID
            );
        };

    } // internal

    constexpr auto HeaderBuffSize = internal::calc_header_size();
    constexpr auto HeaderRemainderSize = HeaderBuffSize - ( 32 + ctrader::data::metadata::MetadataSize );

    struct header_t {
        union {
            char data[ HeaderBuffSize ];
            struct {
                char meta_32a[ ctrader::data::metadata::MetadataSize ];
                char timestamp_32a[32]; // |52=SendingTimeFormat|49=
                char remainder[ HeaderRemainderSize  ];
            };
        };
    };

} // ctrader::data::header