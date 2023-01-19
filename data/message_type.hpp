#pragma once

#include <string>

#include "tools/numbers.hpp"
#include "header.hpp"

namespace ctrader::data::message_type {

    enum class MSG : uint8_t {
        TEST_REQ,
        LOGON, 
        MD_REQ_SUB_DEPTH, MD_REQ_SUB_SPOT, MD_REQ_UNSUB_DEPTH, MD_REQ_UNSUB_SPOT, 
    };

    enum class CONN : uint8_t {
        QUOTE, TRADE
    };

    using field_t = struct {
        std::string key;
        std::string value;
    };

    namespace internal{
        using namespace ctrader::settings;
        using namespace ctrader::data;

        constexpr char MSG_LOOKUP[] = {
            '1', 
            'A', 
            'V', 'V', 'V', 'V'
        };

        constexpr std::string_view CONN_LOOKUP[2] = { "QUOTE", "TRADE" };

        template<MSG T> struct body_t{};

        consteval uint16_t calc_body_length_header_part(){
            return (
                4 + 5 + // 35=?|34=?
                32 + // |52=timestamp|..
                header::HeaderRemainderSize +
                1 // |
            );
        };

    }

    constexpr uint16_t BodyLengthHeaderPart = internal::calc_body_length_header_part();
    constexpr uint8_t TestReqIDMinsize = (BodyLengthHeaderPart + 5) < 100 ? (100 - (BodyLengthHeaderPart + 5)) + 1 : 4; 

} // ctrader::data::message_type




namespace ctrader::data::message_type::internal{

using namespace ctrader::settings;
using namespace ctrader::tools;

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
            char MDReqID[5 + FieldIDDigitSize];  // |262={0:FieldIDDigitSize}
            char SubscriptionRequestType[5 + 1]; // |263=?
            char MarketDepth[5 + 1]; // |264=?
            char MarketDepthMDUpdateType[5 + 1]; // |265=1
            char NoMDEntryTypes[5 + 1]; // |267=2
            char MDEntryTypeBid[5 + 1]; // |269=0
            char MDEntryTypeOffer[5 + 1]; // |269=1
            char NoRelatedSym[5 + 1]; // |146=1
            char Symbol[4 + SymbolIDDigitSize]; // |55={0:SymbolIDDigitSize}
        } field;
        char raw[ sizeof(field) ];
    };
};

template<> struct body_t<MSG::MD_REQ_SUB_DEPTH> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_SUB_SPOT> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_UNSUB_DEPTH> : _MD_REQ {};
template<> struct body_t<MSG::MD_REQ_UNSUB_SPOT> : _MD_REQ {};

}

/*

subscribe
*/