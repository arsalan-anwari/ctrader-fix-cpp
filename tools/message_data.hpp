#pragma once

#include <string>
#include <initializer_list>

#include "settings.hpp"
#include "types/message.hpp"

namespace ctrader::tools::message_data {
    
    using namespace ctrader::types::message;
    using namespace ctrader::types::body;
    using namespace ctrader::types::encode;
    using namespace ctrader::tools;
    using namespace ctrader::settings;

    using field_t = struct {
        std::string key;
        std::string value;
    };
    
    template<ENCODE_TYPE T>
    consteval message_t<T> new_message_from_fields( CONN_TYPE conn, std::initializer_list<field_t> fields ){
        message_t<T> buff;
        const char msgType = ENCODE_CHAR[static_cast<u8>(T)];
        const u16 bodylen = ( internal::BodyLengthHeaderPart + sizeof(buff.body.raw));

        static_assert(bodylen >= 100, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) lower than 100!");
        static_assert(bodylen <= 999, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) higher than 999!");
        
        auto bodylenStr = numbers::to_simple_buffer<3>(bodylen);

        std::string data;
        data += std::string("8=FIX.4.4") + __SETTINGS_SOH_STR;

        const field_t HeaderFields[7] = {
            field_t{"9", std::string(bodylenStr.data, bodylenStr.data+bodylenStr.size) },
            field_t{"35", std::string(1, msgType) },
            field_t{"34", std::string(settings::MsgSeqNumDigitSize, '0' ) },
            field_t{"52", std::string(24, '0') },
            field_t{"49", std::string(broker_settings::SenderCompID.data()) },
            field_t{"56", std::string("cServer") },
            field_t{"57", std::string( CONN_STRING[ static_cast<u8>(conn) ].data() ) }
        };
        
        for(u8 i=0; i<7; i++){ data += ( HeaderFields[i].key + "=" + HeaderFields[i].value + __SETTINGS_SOH_STR ); }
        for(const auto& field: fields){ data += (field.key + "=" + field.value + __SETTINGS_SOH_STR ); }

        data += "10=000";
        data += __SETTINGS_SOH_STR;

        std::copy(data.begin(), data.end(), buff.data);

        return buff;
    };

    template<ENCODE_TYPE T>
    consteval message_t<T> new_message_from_type(CONN_TYPE conn);

    // template<> consteval message_t<ENCODE_TYPE::LOGON> new_message_from_type(CONN_TYPE conn)

    template<> consteval message_t<ENCODE_TYPE::HEART_BEAT> new_message_from_type(CONN_TYPE conn){
        std::string testRegID = internal::TestReqIDMinsize == 4 ? "TEST" : std::string(internal::TestReqIDMinsize, 'T').c_str();

        return new_message_from_fields<ENCODE_TYPE::HEART_BEAT>(conn, {
            {"112", testRegID}
        });   
    }

    template<> consteval message_t<ENCODE_TYPE::TEST_REQ> new_message_from_type(CONN_TYPE conn){
        std::string testRegID = internal::TestReqIDMinsize == 4 ? "TEST" : std::string(internal::TestReqIDMinsize, 'T').c_str();

        return new_message_from_fields<ENCODE_TYPE::TEST_REQ>(conn, {
            {"112", testRegID}
        });   
    }

    template<> consteval message_t<ENCODE_TYPE::LOGON> new_message_from_type(CONN_TYPE conn){

        auto hearthBeat = numbers::to_simple_buffer<numbers::digit_count(settings::HearthBeatIntervalSec)>(settings::HearthBeatIntervalSec);
        
        return new_message_from_fields<ENCODE_TYPE::LOGON>(conn, {
            {"98", "0" },
            {"108", std::string(hearthBeat.data, hearthBeat.data+hearthBeat.size) },
            {"141", "Y" },
            {"553", std::string(broker_settings::Username.data()) },
            {"554", std::string(broker_settings::Password.data()) } 
        });

    };

    template<> consteval message_t<ENCODE_TYPE::LOGOUT> new_message_from_type(CONN_TYPE conn){
        std::string logoutText = internal::LogoutTextMinsize == 4 ? "TEXT" : std::string(internal::LogoutTextMinsize, 'T').c_str();

        return new_message_from_fields<ENCODE_TYPE::LOGOUT>(conn, {
            {"58", logoutText}
        });   
    }

    template<> consteval message_t<ENCODE_TYPE::RESEND_REQ> new_message_from_type(CONN_TYPE conn){
        return new_message_from_fields<ENCODE_TYPE::RESEND_REQ>(conn, {
            {"7", std::string(settings::MsgSeqNumDigitSize, '0' ) },
            {"16", std::string(settings::MsgSeqNumDigitSize, '0' ) }
        });
    }

    template<> consteval message_t<ENCODE_TYPE::MD_REQ_SINGLE> new_message_from_type(CONN_TYPE conn){
        return new_message_from_fields<ENCODE_TYPE::MD_REQ_SINGLE>(conn, {
            {"262", std::string(settings::KeySize, '0')},
            {"263", "0"}, {"264", "0"}, {"265", "1"}, {"267", "2"}, {"269", "0"}, {"269", "1"}, 
            {"146", "0"}, {"55", std::string(20, '0') }
        });   
    }


} // ctrader::tools::message_data