#pragma once

#include "encode_impl.hpp"

namespace ctrader::parser::encode {

using namespace ctrader::types::encode;

template <CONN_TYPE C>
struct Encoder {
   
    template<ENCODE_TYPE M, typename... FIELD_TYPE> 
    inline void encode_message(FIELD_TYPE... fields) { 
        ctrader::parser::encode_impl::prepare_message<M, C>( msg_seq_num, fields... ); 
        advance_seq_num();
    };

    inline void advance_seq_num() { msg_seq_num++; };

    inline void reduce_seq_num() { 
        using namespace ctrader::tools::numbers;
        i32 is_decrementable = op::gte(msg_seq_num, 1);
        msg_seq_num -= is_decrementable;
    };

    inline void reset_seq_num() { msg_seq_num = 1; };

    inline i64 get_seq_num() { return msg_seq_num; };

private:
        i64 msg_seq_num = 1;
};

} // ctrader::parser::encode
