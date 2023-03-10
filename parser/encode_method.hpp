#pragma once

#include <cstring>
#include <utility>
#include <concepts>

#include "types/message.hpp"
#include "types/encode.hpp"
#include "tools/message_data.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode::methods {

    using namespace ctrader::types::encode;
    using namespace ctrader::types::message;
    using namespace ctrader::types::symbol;
    using namespace ctrader::types::concepts;

    using namespace ctrader::tools;
    using namespace ctrader::tools::message_data;
    using namespace ctrader::parser::checksum;
    using namespace ctrader::settings;

namespace {
    #define __PREPARE_HEADER() \
        numbers::to_string( buffer.header.field.MsgSeqNum+4, buffer.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
        datetime::current_timestamp( buffer.header.field.SendingTime ); \

    #define __PREPARE_FOOTER() \
        const u8 cs = calc_checksum<sizeof(buffer.data)-7>(buffer.data); \
        std::memset(buffer.checksum+4, '0', 3); \
        numbers::to_string( buffer.checksum+4, buffer.checksum+4+3, cs); \

    #define __PREPARE_DEFAULT() \
        numbers::to_string( buffer.header.field.MsgSeqNum+4, buffer.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
        datetime::current_timestamp( buffer.header.field.SendingTime ); \
        const u8 cs = calc_checksum<sizeof(buffer.data)-7>(buffer.data); \
        std::memset(buffer.checksum+4, '0', 3); \
        numbers::to_string( buffer.checksum+4, buffer.checksum+4+3, cs); \
}

    template<CONN_TYPE C, ENCODE_TYPE T>
    struct encode_method{
        message_t<T> buffer = new_message_from_type<T>(C);
        template<typename... FIELD_TYPE> void prepare( u64 msg_seq_num, FIELD_TYPE... fields ){ __PREPARE_DEFAULT(); };
    };

    template<CONN_TYPE C>
    struct encode_method<C, ENCODE_TYPE::RESEND_REQ> {
        message_t<ENCODE_TYPE::RESEND_REQ> buffer = new_message_from_type<ENCODE_TYPE::RESEND_REQ>(C);

        template<typename T1, typename T2 = T1> requires (std::unsigned_integral<T1> && std::unsigned_integral<T2>)
        void prepare( u64 msg_seq_num, const T1 seq_num_begin, const T2 seq_num_end ){
            __PREPARE_HEADER();

            std::memset( buffer.body.field.BeginSeqNo+3, '0', MsgSeqNumDigitSize );
            std::memset( buffer.body.field.EndSeqNo+4, '0', MsgSeqNumDigitSize );

            numbers::to_string(buffer.body.field.BeginSeqNo+3, buffer.body.field.BeginSeqNo+3+MsgSeqNumDigitSize, seq_num_begin);
            numbers::to_string(buffer.body.field.EndSeqNo+4, buffer.body.field.EndSeqNo+4+MsgSeqNumDigitSize, seq_num_end);

            __PREPARE_FOOTER();
        }; 
    };

    template<CONN_TYPE C>
    struct encode_method<C, ENCODE_TYPE::MD_REQ_SINGLE> {
        message_t<ENCODE_TYPE::MD_REQ_SINGLE> buffer = new_message_from_type<ENCODE_TYPE::MD_REQ_SINGLE>(C);

        void prepare(u64 msg_seq_num, const char* mdReqId, const SUB_TYPE sub_type, const DEPTH_TYPE depth_type, const SYMBOL symbol){
            static_assert( C == CONN_TYPE::QUOTE, "ENCODE_TYPE:MD_REQ_SINGLE Cannot be used with CONN_TYPE::TRADE! Use CONN_TYPE::QUOTE instead..." );
            
            __PREPARE_HEADER();

            std::memcpy(buffer.body.field.MDReqID+5, mdReqId, KeySize); //262={'':KeySize} 

            buffer.body.field.SubscriptionRequestType[ 5 ] = static_cast<char>(sub_type); // |263=?
            buffer.body.field.MarketDepth[ 5 ] = static_cast<char>(depth_type); // |264=?

            // |55={0-9:SYMBOL_DIGIT_SIZE}
            std::memset(buffer.body.field.Symbol+4, '0', 20);
            u64 symbol_id = static_cast<u64>(symbol);
            std::memcpy(
                buffer.body.field.Symbol+4+(20-SYMBOL_DIGIT_SIZE[symbol_id]), 
                SYMBOL_STRING[symbol_id], 
                SYMBOL_DIGIT_SIZE[symbol_id]
            );

            __PREPARE_FOOTER();
        }
    };

}