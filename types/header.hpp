#pragma once

#include "settings.hpp"

namespace ctrader::types::header {

    using namespace ctrader::settings;
    
    namespace {

        using namespace ctrader::types::numbers; 
    

        consteval u32 calc_header_size(){
            return (
                2 + 7 + // 8=FIX.4.4
                3 + 3 + // |9=???
                4 + 1 + // |35=?
                4 + MsgSeqNumDigitSize + // |34={0:MsgSeqNumDigitSize}
                24 + 4 + // |52=SendingTimeFormat -> 24-32
                broker_settings::SenderCompID.size() + 4 + // |49=SenderCompID
                7 + 4 + // |56=TargetCompID
                5 + 4 + // |57=TargetSubID
                5 + 4 // |50=SenderSubID
            );
        };

        consteval u32 calc_header_remainder_size(u32 total){
            u32 begin = 
                2 + 7 + // 8=FIX.4.4
                3 + 3 + // |9=???
                4 + 1 + // |35=?
                4 + MsgSeqNumDigitSize + // |34={0:MsgSeqNumDigitSize}
                32; // |52=SendingTimeFormat -> 24-32

            return total - begin;
        }

    } // internal

    constexpr auto HeaderBuffSize = calc_header_size();
    constexpr auto HeaderRemainderSize = calc_header_remainder_size(HeaderBuffSize);

    struct header_t {
        union {
            struct {
                char BeginString[ 2 + 7 ]; // 8=FIX.4.4
                char BodyLength[3 + 3]; // |9=???
                char MsgType[4 + 1]; // |35=?
                char MsgSeqNum[4 + MsgSeqNumDigitSize]; // |34={0:MsgSeqNumDigitSize}
                char timestamp_32a[32]; // |52=SendingTimeFormat|49=
                char SenderCompID[ broker_settings::SenderCompID.size() ]; // SenderCompID
                char TargetCompID[ 4 + 7 ]; // |56=cServer
                char TargetSubID[4 + 5]; // |57={QUOTE?TRADE}
            } field;
            char raw[ sizeof(field) ];
        };
    };

} // ctrader::types::header