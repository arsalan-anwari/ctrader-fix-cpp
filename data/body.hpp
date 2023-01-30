#pragma once

#include "types/message.hpp"
#include "tools/numbers.hpp"
#include "header.hpp"

namespace ctrader::data::body {


    namespace internal{
        using namespace ctrader::settings;
        using namespace ctrader::data::header;
        using namespace ctrader::types::message;

        template<MSG T> struct body_t{};

        consteval uint16_t calc_body_length_header_part(){
            return (
                4 + 5 + // 35=?|34=?
                32 + // |52=timestamp|..
                HeaderRemainderSize +
                1 // |
            );
        };

    }

    constexpr uint16_t BodyLengthHeaderPart = internal::calc_body_length_header_part();
    constexpr uint8_t TestReqIDMinsize = (BodyLengthHeaderPart + 5) < 100 ? (100 - (BodyLengthHeaderPart + 5)) + 1 : 4; 

} // ctrader::data::message




namespace ctrader::data::body::internal{

using namespace ctrader::settings;
using namespace ctrader::tools;
using namespace ctrader::types::message;

template<> struct body_t<MSG::LOGON> {
    union{
        struct {
            char EncryptMethod[5];
            char HeartBtInt[5 + numbers::digit_count( HearthBeatIntervalSec )];
            char ResetSeqNumFlag[6];
            char Username[5 + broker_settings::Username.size()];
            char Password[5 + broker_settings::Password.size()];
        } field;
        char raw[ sizeof(field) ];
    };
};

template<> struct body_t<MSG::TEST_REQ> {
    union{
        struct {
            char TestReqID[ TestReqIDMinsize + 5 ];
        } field;
        char raw[ sizeof(field) ];
    };
};

struct _MD_REQ {
    union{
        struct {
            char MDReqID[5 + KeySize];  // |262={0:KeySize}
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

template<> struct body_t<MSG::MD_REQ_SUB_DEPTH> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_SUB_SPOT> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_UNSUB_DEPTH> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_UNSUB_SPOT> : _MD_REQ {};

}
