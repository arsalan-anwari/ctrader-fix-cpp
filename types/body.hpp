#pragma once

#include "types/encode.hpp"
#include "tools/numbers.hpp"
#include "header.hpp"


namespace internal {
    using namespace ctrader::types::numbers; 
    using namespace ctrader::types::encode;
    using namespace ctrader::types::header;
    using namespace ctrader::settings;
    using namespace ctrader::tools;

    consteval u16 calc_body_length_header_part(){
        return (
            4 + 5 + // 35=?|34=?
            32 + // |52=timestamp|..
            HeaderRemainderSize +
            1 // |
        );
    };

    constexpr u16 BodyLengthHeaderPart = calc_body_length_header_part();
    constexpr u8 TestReqIDMinsize = (BodyLengthHeaderPart + 5) < 100 ? (100 - (BodyLengthHeaderPart + 5)) + 1 : 4; 
    constexpr u8 LogoutTextMinsize = TestReqIDMinsize + 1;
    constexpr u8 HearthBeatIntervalSecSize = numbers::digit_count( HearthBeatIntervalSec );
}

namespace ctrader::types::body {

    using namespace ctrader::types::numbers; 
    using namespace ctrader::types::encode;
    using namespace ctrader::settings;

    template<ENCODE_TYPE T> struct body_t{};

    // template<> struct body_t<ENCODE_TYPE::> {
    //     union {
    //         struct {
                
    //         } field;
    //         char raw [ sizeof(field) ];
    //     };
    // };

    template<> struct body_t<ENCODE_TYPE::HEART_BEAT> {
        union {
            struct {
                char TestReqID[ internal::TestReqIDMinsize + 5 ]; // |112={'':TestReqIDMinsize}
            } field;
            char raw [ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::TEST_REQ> : body_t<ENCODE_TYPE::HEART_BEAT> {};

    template<> struct body_t<ENCODE_TYPE::LOGON> {
        union{
            struct {
                char EncryptMethod[4 + 1]; // |98=0
                char HeartBtInt[5 + internal::HearthBeatIntervalSecSize]; // |108={0:HearthBeatIntervalSecSize}
                char ResetSeqNumFlag[5 + 1]; // |141=Y
                char Username[5 + broker_settings::Username.size()]; // |553={Username}
                char Password[5 + broker_settings::Password.size()]; // |554={Password}
            } field;
            char raw[ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::LOGOUT> {
        union {
            struct {
                char Text[ internal::LogoutTextMinsize + 4 ]; // |58={'':LogoutTextMinsize}
            } field;
            char raw [ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::RESEND_REQ> {
        union {
            struct {
                char BeginSeqNo[ 3 + settings::MsgSeqNumDigitSize ]; // |7={0:MsgSeqNumDigitSize}
                char EndSeqNo[ 4 + settings::MsgSeqNumDigitSize ]; // |16={0:MsgSeqNumDigitSize}
            } field;
            char raw [ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::MD_REQ_SINGLE> {
        union{
            struct {
                char MDReqID[5 + settings::KeySize];  // |262={0:KeySize}
                char SubscriptionRequestType[5 + 1]; // |263=?
                char MarketDepth[5 + 1]; // |264=?
                char MarketDepthMDUpdateType[5 + 1]; // |265=1
                char NoMDEntryTypes[5 + 1]; // |267=2
                char MDEntryTypeBid[5 + 1]; // |269=0
                char MDEntryTypeOffer[5 + 1]; // |269=1
                char NoRelatedSym[5 + 1]; // |146=1
                char Symbol[4 + 20]; // |55={0:20}
            } field;
            char raw[ sizeof(field) ];
        };
    };

} // ctrader::types::body

