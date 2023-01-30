#pragma once

#include <string_view>
#include <stdint.h>

#include "data/field_id.hpp"

namespace ctrader::settings {

    constexpr uint8_t           HearthBeatIntervalSec = 30;
    constexpr uint8_t           MsgSeqNumDigitSize = 18; // range {1 ... 18}
    constexpr uint8_t           KeySize = ctrader::data::field_id::KeySize;
    constexpr char              SOHChar = '|'; //mostly used for debugging output, set to '\1' on release
    #define __SETTINGS_SOH "|"

    namespace broker {

        namespace ic_markets_demo {

            constexpr std::string_view  Username = "8536054";
            constexpr std::string_view  Password = "RGZC9bBP68VqyAY";
            constexpr std::string_view  SenderCompID = "demo.icmarkets.8536054";
            
        } // ic_markets_demo

    } // broker

    namespace broker_settings = ctrader::settings::broker::ic_markets_demo;

} // ctrader::settings




