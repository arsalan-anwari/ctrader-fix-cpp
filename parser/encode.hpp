#pragma once

#include "encode_method.hpp"

namespace ctrader::parser::encode {

using namespace ctrader::parser::encode::methods;

template <CONN_TYPE C> 
struct Encoder {
    Encoder(){};
       
    template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    void encode_message(FIELD_TYPE... fields) { 
        if constexpr( M == ENCODE_TYPE::HEART_BEAT ) { HEART_BEAT.prepare(msg_seq_num, fields...); }
        else if constexpr( M == ENCODE_TYPE::TEST_REQ ) { TEST_REQ.prepare(msg_seq_num, fields...); }
        else if constexpr( M == ENCODE_TYPE::LOGON ) { LOGON.prepare(msg_seq_num, fields...); }
        else if constexpr( M == ENCODE_TYPE::LOGOUT ) { LOGOUT.prepare(msg_seq_num, fields...); }
        else if constexpr( M == ENCODE_TYPE::RESEND_REQ ) { RESEND_REQ.prepare(msg_seq_num, fields...); }
        else if constexpr( M == ENCODE_TYPE::MD_REQ_SINGLE ) { MD_REQ_SINGLE.prepare(msg_seq_num, fields...); }
        advance_seq_num(); 
    };

    inline __attribute__((always_inline)) void advance_seq_num() { msg_seq_num++; };
    inline __attribute__((always_inline)) void reduce_seq_num() { 
        using namespace ctrader::tools::numbers;
        i32 is_decrementable = op::gte(msg_seq_num, 1);
        msg_seq_num -= is_decrementable;
    };
    inline __attribute__((always_inline)) void reset_seq_num() { msg_seq_num = 1; };
    inline __attribute__((always_inline)) u64 get_seq_num() { return msg_seq_num; };

    template<ENCODE_TYPE M> message_t<M> const& get_message_buff() const{
        if constexpr( M == ENCODE_TYPE::HEART_BEAT ) { return HEART_BEAT.buffer; }
        else if constexpr( M == ENCODE_TYPE::TEST_REQ ) { return TEST_REQ.buffer; }
        else if constexpr( M == ENCODE_TYPE::LOGON ) { return LOGON.buffer; }
        else if constexpr( M == ENCODE_TYPE::LOGOUT ) { return LOGOUT.buffer; }
        else if constexpr( M == ENCODE_TYPE::RESEND_REQ ) { return RESEND_REQ.buffer; }
        else if constexpr( M == ENCODE_TYPE::MD_REQ_SINGLE ) { return MD_REQ_SINGLE.buffer; }
    }

private:
    u64 msg_seq_num = 1;
    encode_method<C, ENCODE_TYPE::HEART_BEAT> HEART_BEAT;
    encode_method<C, ENCODE_TYPE::TEST_REQ> TEST_REQ;

    encode_method<C, ENCODE_TYPE::LOGON> LOGON;
    encode_method<C, ENCODE_TYPE::LOGOUT> LOGOUT;
    encode_method<C, ENCODE_TYPE::RESEND_REQ> RESEND_REQ;

    encode_method<C, ENCODE_TYPE::MD_REQ_SINGLE> MD_REQ_SINGLE;
    
};

} // ctrader::parser::encode
