#pragma once

#include "types/encode.hpp"
#include "tools/numbers.hpp"
#include "header.hpp"


namespace {
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
    constexpr u8 HearthBeatIntervalSecSize = numbers::digit_count( HearthBeatIntervalSec );
}

namespace ctrader::types::body {

    using namespace ctrader::types::numbers; 
    using namespace ctrader::types::encode;
    using namespace ctrader::settings;

    template<ENCODE_TYPE T> struct body_t{};

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

    template<> struct body_t<ENCODE_TYPE::LOGON> {
        union{
            struct {
                char EncryptMethod[5];
                char HeartBtInt[5 + HearthBeatIntervalSecSize];
                char ResetSeqNumFlag[6];
                char Username[5 + broker_settings::Username.size()];
                char Password[5 + broker_settings::Password.size()];
            } field;
            char raw[ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::TEST_REQ> {
        union{
            struct {
                char TestReqID[ TestReqIDMinsize + 5 ];
            } field;
            char raw[ sizeof(field) ];
        };
    };

    template<> struct body_t<ENCODE_TYPE::MD_REQ_SUB_DEPTH> : _MD_REQ {};
    template<> struct body_t<ENCODE_TYPE::MD_REQ_SUB_SPOT> : _MD_REQ {};
    template<> struct body_t<ENCODE_TYPE::MD_REQ_UNSUB_DEPTH> : _MD_REQ {};
    template<> struct body_t<ENCODE_TYPE::MD_REQ_UNSUB_SPOT> : _MD_REQ {};


} // ctrader::types::body

