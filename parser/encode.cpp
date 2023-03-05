#include <cstring>
#include <utility>
#include <exception>

#include "encode.hpp"

#include "data/message_data.hpp"
#include "types/encode.hpp"
#include "types/symbol.hpp"

#include "tools/numbers.hpp"
#include "tools/datetime.hpp"
#include "tools/memory.hpp"

#include "checksum.hpp"
#include "settings.hpp"

namespace ctrader::parser::encode {

    using namespace ctrader::data::message_data;
    using namespace ctrader::types::encode;
    using namespace ctrader::types::symbol;
    using namespace ctrader::tools;
    using namespace ctrader::parser::checksum;
    using namespace ctrader::settings;

// define some macros to reduce boiler plate code...
namespace {
    #define __PREPARE_HEADER(MSG_TYPE) \
        numbers::to_string( MSG_TYPE.header.field.MsgSeqNum+4, MSG_TYPE.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
        datetime::current_timestamp( MSG_TYPE.header.field.timestamp_32a ); \

    #define __PREPARE_FOOTER(MSG_TYPE) \
        const u8 cs = calc_checksum<sizeof(MSG_TYPE.data)-7>(MSG_TYPE.data); \
        std::memset(MSG_TYPE.checksum+4, '0', 3); \
        numbers::to_string( MSG_TYPE.checksum+4, MSG_TYPE.checksum+4+3, cs); \

    #define __PREPARE_DEFAULT(MSG_TYPE) \
        numbers::to_string( MSG_TYPE.header.field.MsgSeqNum+4, MSG_TYPE.header.field.MsgSeqNum+MsgSeqNumDigitSize+4, msg_seq_num ); \
        datetime::current_timestamp( MSG_TYPE.header.field.timestamp_32a ); \
        const u8 cs = calc_checksum<sizeof(MSG_TYPE.data)-7>(MSG_TYPE.data); \
        std::memset(MSG_TYPE.checksum+4, '0', 3); \
        numbers::to_string( MSG_TYPE.checksum+4, MSG_TYPE.checksum+4+3, cs); \

    #define __CREATE_EMPTY_PARSER_IMPL( MSG_TYPE, FUNC_DEF ) \
        template<> template<> void Encoder<CONN_TYPE::QUOTE>::prepare_message<MSG_TYPE>FUNC_DEF \
        template<> template<> void Encoder<CONN_TYPE::TRADE>::prepare_message<MSG_TYPE>FUNC_DEF \
        
    #define __CREATE_DEFAULT_PARSER_IMPL( MSG_TYPE ) __CREATE_EMPTY_PARSER_IMPL( ENCODE_TYPE::MSG_TYPE, (void){ __PREPARE_DEFAULT( MSG_TYPE ); } )
}

// Start of cpp
    template <CONN_TYPE C> 
    Encoder<C>::Encoder(): 
        LOGON( new_message_from_type<ENCODE_TYPE::LOGON>(C) ),
        TEST_REQ( new_message_from_type<ENCODE_TYPE::TEST_REQ>(C) ),
        MD_REQ_SUB_DEPTH( new_message_from_type<ENCODE_TYPE::MD_REQ_SUB_DEPTH>(C) )
    {}

    template <CONN_TYPE C> template<ENCODE_TYPE M> 
    message_t<M> const& Encoder<C>::get_message_buff() const {
        if constexpr( M == ENCODE_TYPE::LOGON ){ return LOGON;} 
        else if constexpr( M == ENCODE_TYPE::TEST_REQ ){ return TEST_REQ; }
        else if constexpr( M == ENCODE_TYPE::MD_REQ_SUB_DEPTH ){ return MD_REQ_SUB_DEPTH; }
    }

// create prepare_message specializations

    template <CONN_TYPE C> template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    void Encoder<C>::prepare_message(FIELD_TYPE... fields){}

    __CREATE_DEFAULT_PARSER_IMPL( LOGON );
    __CREATE_DEFAULT_PARSER_IMPL( TEST_REQ );

    template<> template<> void Encoder<CONN_TYPE::QUOTE>::prepare_message<ENCODE_TYPE::MD_REQ_SUB_DEPTH>(
        const SYMBOL symbol, const char* mdReqId
    ){
        
        __PREPARE_HEADER(MD_REQ_SUB_DEPTH);

        std::memcpy(MD_REQ_SUB_DEPTH.body.field.MDReqID+5, mdReqId, KeySize);
        std::memset(MD_REQ_SUB_DEPTH.body.field.Symbol+4, '0', 20);

        u64 symbol_id = static_cast<u64>(symbol);
        std::memcpy(
            MD_REQ_SUB_DEPTH.body.field.Symbol+4+(20-SYMBOL_DIGIT_SIZE[symbol_id]), 
            SYMBOL_STRING[symbol_id], 
            SYMBOL_DIGIT_SIZE[symbol_id]
        );

        __PREPARE_FOOTER(MD_REQ_SUB_DEPTH);
    };


// explicit template specialization to prevent linker to complain it cannot find the functions. 
    template struct Encoder<CONN_TYPE::QUOTE>;
    template struct Encoder<CONN_TYPE::TRADE>;
    template message_t<ENCODE_TYPE::LOGON> const& Encoder<CONN_TYPE::QUOTE>::get_message_buff() const;
    template message_t<ENCODE_TYPE::LOGON> const& Encoder<CONN_TYPE::TRADE>::get_message_buff() const;
    template message_t<ENCODE_TYPE::TEST_REQ> const& Encoder<CONN_TYPE::QUOTE>::get_message_buff() const;
    template message_t<ENCODE_TYPE::TEST_REQ> const& Encoder<CONN_TYPE::TRADE>::get_message_buff() const;
    template message_t<ENCODE_TYPE::MD_REQ_SUB_DEPTH> const& Encoder<CONN_TYPE::QUOTE>::get_message_buff() const;
    template message_t<ENCODE_TYPE::MD_REQ_SUB_DEPTH> const& Encoder<CONN_TYPE::TRADE>::get_message_buff() const;
    template void Encoder<CONN_TYPE::TRADE>::prepare_message<ENCODE_TYPE::MD_REQ_SUB_DEPTH>( const SYMBOL symbol, const char* mdReqId );

} // ctrader::parser::encode