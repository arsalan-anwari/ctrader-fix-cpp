#pragma once

#include <string>
#include <initializer_list>

#include "settings.hpp"
#include "message.hpp"

namespace ctrader::data::message_data {
    

    namespace internal{
        using namespace ctrader::data::message;
        using namespace ctrader::data::body;
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
            const u16 bodylen = ( BodyLengthHeaderPart + sizeof(buff.body.raw));
    
            static_assert(bodylen >= 100, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) lower than 100!");
            static_assert(bodylen <= 999, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) higher than 999!");
            
            auto bodylenStr = numbers::to_simple_buffer<3>(bodylen);

            std::string data;
            data += std::string("8=FIX.4.4") + SOHChar;

            const field_t HeaderFields[7] = {
                field_t{"9", std::string(bodylenStr.data, bodylenStr.data+bodylenStr.size) },
                field_t{"35", std::string(1, msgType) },
                field_t{"34", std::string(MsgSeqNumDigitSize, '0' ) },
                field_t{"52", std::string(24, '0') },
                field_t{"49", std::string(broker_settings::SenderCompID.data()) },
                field_t{"56", std::string("cServer") },
                field_t{"57", std::string( CONN_STRING[ static_cast<u8>(conn) ].data() ) }
            };
            
            for(u8 i=0; i<7; i++){ data += ( HeaderFields[i].key + "=" + HeaderFields[i].value + SOHChar ); }
            for(const auto& field: fields){ data += (field.key + "=" + field.value + SOHChar ); }

            data += "10=000";
            data += SOHChar;

            std::copy(data.begin(), data.end(), buff.data);

            return buff;
        };

        template<ENCODE_TYPE T>
        consteval message_t<T> new_message_from_type(CONN_TYPE conn);

        template<> consteval message_t<ENCODE_TYPE::LOGON> new_message_from_type(CONN_TYPE conn){

            auto hearthBeat = numbers::to_simple_buffer<numbers::digit_count(HearthBeatIntervalSec)>(HearthBeatIntervalSec);
            
            return new_message_from_fields<ENCODE_TYPE::LOGON>(conn, {
                {"98", "0" },
                {"108", std::string(hearthBeat.data, hearthBeat.data+hearthBeat.size) },
                {"141", "Y" },
                {"553", std::string(broker_settings::Username.data()) },
                {"554", std::string(broker_settings::Password.data()) } 
            });

        };

        template<> consteval message_t<ENCODE_TYPE::TEST_REQ> new_message_from_type(CONN_TYPE conn){
            std::string testRegID = TestReqIDMinsize == 4 ? "TEST" : std::string(TestReqIDMinsize, 'T').c_str();

            return new_message_from_fields<ENCODE_TYPE::TEST_REQ>(conn, {
                {"112", testRegID}
            });   
        }

        template<> consteval message_t<ENCODE_TYPE::MD_REQ_SUB_DEPTH> new_message_from_type(CONN_TYPE conn){
            
            return new_message_from_fields<ENCODE_TYPE::MD_REQ_SUB_DEPTH>(conn, {
                {"262", std::string(KeySize, '0')},
                {"263", "1"}, {"264", "0"}, {"265", "1"}, {"267", "2"}, {"269", "0"}, {"269", "1"}, 
                {"146", "1"}, {"55", std::string(20, '0') }
            });   
        }



    } // internal 

    namespace quote {
        using namespace ctrader::types::encode;

        constinit auto LOGON = internal::new_message_from_type<ENCODE_TYPE::LOGON>(CONN_TYPE::QUOTE);
        constinit auto TEST_REQ = internal::new_message_from_type<ENCODE_TYPE::TEST_REQ>(CONN_TYPE::QUOTE);
        constinit auto MD_REQ_SUB_DEPTH = internal::new_message_from_type<ENCODE_TYPE::MD_REQ_SUB_DEPTH>(CONN_TYPE::QUOTE);
    } // quote 

    namespace trade {
        using namespace ctrader::types::encode;

        constinit auto LOGON = internal::new_message_from_type<ENCODE_TYPE::LOGON>(CONN_TYPE::TRADE);
        constinit auto TEST_REQ = internal::new_message_from_type<ENCODE_TYPE::TEST_REQ>(CONN_TYPE::TRADE);
    } // trade 

 

    



} // ctrader::data::message_data