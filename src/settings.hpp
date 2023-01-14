#pragma once

#include <string_view>
#include <stdint.h>

namespace ctrader::settings {

    constexpr uint8_t           HearthBeatIntervalSec = 30;
    constexpr uint16_t          MsgThroughputSec = 115; // range {1 ... 1000}
    constexpr bool              SSL = false;
    constexpr char              SOHChar = '|'; //mostly used for debugging output, set to '\1' on release
    constexpr std::string_view  SymbolName = "1";
    constexpr uint8_t           FieldIDSize = 6;

    namespace broker {

        namespace ic_markets_demo {

        constexpr std::string_view  Host = "h51.p.ctrader.com";
        constexpr std::string_view  Port = "5201";
        constexpr std::string_view  Username = "8536054";
        constexpr std::string_view  Password = "RGZC9bBP68VqyAY";

        constexpr std::string_view  BeginString = "FIX.4.4";
        constexpr std::string_view  SenderCompID = "demo.icmarkets.8536054";
        constexpr std::string_view  SenderSubID = "QUOTE";
        constexpr std::string_view  TargetCompID = "cServer";
        constexpr std::string_view  TargetSubID = "QUOTE";
        
        // see manual why this format is used and not strftime(3) format
        // only support for options { '%YYY', '%m', '%d', '%H', '%M', '%S', '%fffff' }
        // duplicate options will result in undefined behaviour
        // all options need to be present at least once otherwise compile time error will occur
        // format size cannot be larger than 28 otherwise compile time error raised
        constexpr std::string_view  SendingTimeFormat = "%YYY%m%d-%H:%M:%S.%fffff";

        } // ic_markets_demo

    } // broker

    namespace broker_settings = ctrader::settings::broker::ic_markets_demo;

} // ctrader::settings




