#pragma once

#include "types/message.hpp"
#include "types/encode.hpp"

namespace ctrader::parser::encode {

using namespace ctrader::types::encode;
using namespace ctrader::types::message;

template <CONN_TYPE C> 
struct Encoder {
    
    Encoder();
   
    template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    void encode_message(FIELD_TYPE... fields) { prepare_message<M>( fields... );  advance_seq_num(); };

    inline __attribute__((always_inline)) void advance_seq_num() { msg_seq_num++; };
    inline __attribute__((always_inline)) void reduce_seq_num() { 
        using namespace ctrader::tools::numbers;
        i32 is_decrementable = op::gte(msg_seq_num, 1);
        msg_seq_num -= is_decrementable;
    };
    inline __attribute__((always_inline)) void reset_seq_num() { msg_seq_num = 1; };
    inline __attribute__((always_inline)) i64 get_seq_num() { return msg_seq_num; };

    template<ENCODE_TYPE M> message_t<M> const& get_message_buff() const;

private:
    i64 msg_seq_num = 1;
    message_t<ENCODE_TYPE::LOGON> LOGON;
    message_t<ENCODE_TYPE::TEST_REQ> TEST_REQ;
    message_t<ENCODE_TYPE::MD_REQ_SUB_DEPTH> MD_REQ_SUB_DEPTH;

    template <CONN_TYPE> friend struct Encoder;

    template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    void prepare_message(FIELD_TYPE... fields);
    
};

} // ctrader::parser::encode
