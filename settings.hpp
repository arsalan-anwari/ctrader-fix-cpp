#pragma once

#include <string_view>

#include "types/numbers.hpp"

namespace ctrader::settings {
    using namespace ctrader::types::numbers;

    #define __SETTINGS_ENABLE 1
    #define __SETTINGS_DISABLE 1

    constexpr u8           HearthBeatIntervalSec = 30;
    constexpr u8           MsgSeqNumDigitSize = 18; // range {1 ... 18}
    constexpr u8           KeySize = 6;
    constexpr u8           DecodeBufferSize = 32; 
   
    #define __SETTINGS_SOH_STR "|"
    #define __SETTINGS_SOH '|'
    #define __SETTINGS_ALLOW_RESTRICTIONS __SETTINGS_ENABLE

    namespace broker {

        namespace ic_markets_demo {

            constexpr std::string_view  Username = "8536054";
            constexpr std::string_view  Password = "RGZC9bBP68VqyAY";
            constexpr std::string_view  SenderCompID = "demo.icmarkets.8536054";
            
        } // ic_markets_demo

    } // broker

    namespace broker_settings = ctrader::settings::broker::ic_markets_demo;

} // ctrader::settings




