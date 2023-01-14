#pragma once

#include <initializer_list>

#include "message.hpp"
#include "tools/protocol.hpp"

namespace ctrader::data::message_data {
    using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;

    namespace internal{
        using namespace ctrader::data::message;
        using namespace ctrader::data::message_type;
        using namespace ctrader::data::timestamp;
        using namespace ctrader::tools;
        using namespace ctrader::settings;
        
        template<MSG_TYPE T>
        consteval message_t<T> new_message_from_fields( std::initializer_list<field_t> fields){
            message_t<T> buff;
            const char msgType = ctrader::data::message_type::internal::MSG_TYPE_LOOKUP[static_cast<uint8_t>(T)];
            const uint16_t bodylen = ( BodyLengthHeaderPart + sizeof(buff.body.data));
            static_assert(bodylen >= 100, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) lower than 100!");
            static_assert(bodylen <= 999, "Cannot generate message_data as some messages have a 'BodyLength' field value (9=...) higher than 999!");
            auto meta = metadata::internal::metadata_from_type( metadata::MetadataInfo.offset, msgType, bodylen, metadata::MetadataInfo.offset);
            
            std::string data = std::string(meta.data, meta.data+meta.size);
            
            const field_t HeaderFields[5] = {
                field_t{"52", std::string(TimestampBuffer.data, TimestampBuffer.data+TimestampSize) },
                field_t{"49", std::string(broker_settings::SenderCompID.data()) },
                field_t{"56", std::string(broker_settings::TargetCompID.data()) },
                field_t{"57", std::string(broker_settings::TargetSubID.data()) },
                field_t{"50", std::string(broker_settings::SenderSubID.data()) }
            };
            
            for(uint8_t i=0; i<5; i++){ data += (SOHChar + HeaderFields[i].key + "=" + HeaderFields[i].value); }
            for(const auto& field: fields){ data += (SOHChar + field.key + "=" + field.value); }
            data += SOHChar;
            data += "10=000";
            data += SOHChar;

            std::copy(data.begin(), data.end(), buff.data);

            return buff;
        };

        template<MSG_TYPE T>
        consteval message_t<T> new_message_from_type() {
            static_assert(T != MSG_TYPE::_MD_REQ, 
            "Cannot create Message type 'MD_REQ' as this is a generic interface." 
            "Use types one of type {'MD_REQ_SUB_DEPTH', 'MD_REQ_SUB_SPOT', 'MD_REQ_UNSUB_DEPTH', 'MD_REQ_UNSUB_SPOT'} instead!"
            );

            return message_t<T>{};
        }

        template<> consteval message_t<MSG_TYPE::LOGON> new_message_from_type(){

            auto hearthBeat = numbers::to_simple_buffer<numbers::digit_count(HearthBeatIntervalSec)>(HearthBeatIntervalSec);
            
            return new_message_from_fields<MSG_TYPE::LOGON>({ 
                {"98", "0" },
                {"108", std::string(hearthBeat.data, hearthBeat.data+hearthBeat.size) },
                {"141", "Y" },
                {"553", std::string(broker_settings::Username.data()) },
                {"554", std::string(broker_settings::Password.data()) } 
            });

        };

        template<> consteval message_t<MSG_TYPE::TEST_REQ> new_message_from_type(){
            std::string testRegID = TestReqIDMinsize == 4 ? "TEST" : std::string(TestReqIDMinsize, 'T').c_str();

            return new_message_from_fields<MSG_TYPE::TEST_REQ>({
                {"112", testRegID}
            });   
        }

        template<> consteval message_t<MSG_TYPE::MD_REQ_SUB_DEPTH> new_message_from_type(){
            return new_message_from_fields<MSG_TYPE::MD_REQ_SUB_DEPTH>({
                {"262", std::string(FieldIDSize, '0')},
                {"263", "1"}, {"264", "0"}, {"265", "1"}, {"267", "2"}, {"269", "0"}, {"269", "1"}, 
                {"146", "1"}, {"55", std::string(broker_settings::SymbolName.data()) }
            });   
        }



    } // internal 

    constinit auto LOGON = internal::new_message_from_type<MSG_TYPE::LOGON>();
    constinit auto TEST_REQ = internal::new_message_from_type<MSG_TYPE::TEST_REQ>();
    constinit auto MD_REQ_SUB_DEPTH = internal::new_message_from_type<MSG_TYPE::MD_REQ_SUB_DEPTH>();

} // ctrader::data::message_data