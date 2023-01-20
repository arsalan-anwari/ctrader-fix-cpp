#pragma once

#include <string>
#include <initializer_list>

#include "settings.hpp"
#include "message.hpp"

namespace ctrader::data::message_data {
    

    namespace internal{
        using namespace ctrader::data::message;
        using namespace ctrader::data::body;
        using namespace ctrader::types::message_type;
        using namespace ctrader::tools;
        using namespace ctrader::settings;

        using field_t = struct {
            std::string key;
            std::string value;
        };
        
        template<MSG T>
        consteval message_t<T> new_message_from_fields( CONN conn, std::initializer_list<field_t> fields ){
            message_t<T> buff;
            const char msgType = MSG_LOOKUP[static_cast<uint8_t>(T)];
            const uint16_t bodylen = ( BodyLengthHeaderPart + sizeof(buff.body.raw));
    
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
                field_t{"57", std::string( CONN_LOOKUP[ static_cast<uint8_t>(conn) ].data() ) }
            };
            
            for(uint8_t i=0; i<7; i++){ data += ( HeaderFields[i].key + "=" + HeaderFields[i].value + SOHChar ); }
            for(const auto& field: fields){ data += (field.key + "=" + field.value + SOHChar ); }

            data += "10=000";
            data += SOHChar;

            std::copy(data.begin(), data.end(), buff.data);

            return buff;
        };

        template<MSG T>
        consteval message_t<T> new_message_from_type(CONN conn);

        template<> consteval message_t<MSG::LOGON> new_message_from_type(CONN conn){

            auto hearthBeat = numbers::to_simple_buffer<numbers::digit_count(HearthBeatIntervalSec)>(HearthBeatIntervalSec);
            
            return new_message_from_fields<MSG::LOGON>(conn, {
                {"98", "0" },
                {"108", std::string(hearthBeat.data, hearthBeat.data+hearthBeat.size) },
                {"141", "Y" },
                {"553", std::string(broker_settings::Username.data()) },
                {"554", std::string(broker_settings::Password.data()) } 
            });

        };

        template<> consteval message_t<MSG::TEST_REQ> new_message_from_type(CONN conn){
            std::string testRegID = TestReqIDMinsize == 4 ? "TEST" : std::string(TestReqIDMinsize, 'T').c_str();

            return new_message_from_fields<MSG::TEST_REQ>(conn, {
                {"112", testRegID}
            });   
        }

        template<> consteval message_t<MSG::MD_REQ_SUB_DEPTH> new_message_from_type(CONN conn){
            
            return new_message_from_fields<MSG::MD_REQ_SUB_DEPTH>(conn, {
                {"262", std::string(KeySize, '0')},
                {"263", "1"}, {"264", "0"}, {"265", "1"}, {"267", "2"}, {"269", "0"}, {"269", "1"}, 
                {"146", "1"}, {"55", std::string(20, '0') }
            });   
        }



    } // internal 

    namespace quote {
        using namespace ctrader::types::message_type;

        constinit auto LOGON = internal::new_message_from_type<MSG::LOGON>(CONN::QUOTE);
        constinit auto TEST_REQ = internal::new_message_from_type<MSG::TEST_REQ>(CONN::QUOTE);
        constinit auto MD_REQ_SUB_DEPTH = internal::new_message_from_type<MSG::MD_REQ_SUB_DEPTH>(CONN::QUOTE);
    } // quote 

    namespace trade {
        using namespace ctrader::types::message_type;

        constinit auto LOGON = internal::new_message_from_type<MSG::LOGON>(CONN::TRADE);
        constinit auto TEST_REQ = internal::new_message_from_type<MSG::TEST_REQ>(CONN::TRADE);
    } // trade 

 

    



} // ctrader::data::message_data