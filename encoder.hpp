#pragma once

#include "types/numbers.hpp"
#include "types/message.hpp"
#include "types/encode.hpp"

#include "encoder/encode_method.hpp"
#include "tools/bitwise.hpp"

namespace ctrader {
namespace encode {
    
    template<connection Conn>
    class encoder {
    public:
        template<message T, typename... Field>
        void encode(Field... fields) {
            if constexpr (T == message::heart_beat) { heart_beat.prepare(msg_seq_num, fields...); return; }
            if constexpr (T == message::test_req) { test_req.prepare(msg_seq_num, fields...); return; }
            if constexpr (T == message::logon) { logon.prepare(msg_seq_num, fields...); return; }
            if constexpr (T == message::market_data_req) { market_data_req.prepare(msg_seq_num, fields...); return; }
            advance_seq_num();
        }

        inline void advance_seq_num() { msg_seq_num++; }
        inline void reduce_seq_num() { msg_seq_num -= bitwise::gte(msg_seq_num, 1); }
        inline void reset_seq_num() { msg_seq_num = 1; }
        inline u64 get_seq_num() const { return msg_seq_num; }

        template<message T>
        packet_t<T> const& get_buffer() const {
            if constexpr (T == message::heart_beat) { return heart_beat.buff; }
            if constexpr (T == message::test_req) { return test_req.buff; }
            if constexpr (T == message::logon) { return logon.buff; }
            if constexpr (T == message::market_data_req) { return market_data_req.buff; }
        }

    private:
        u64 msg_seq_num = 1;
        encode_method<Conn, message::heart_beat> heart_beat;
        encode_method<Conn, message::test_req> test_req;
        encode_method<Conn, message::logon> logon;
        encode_method<Conn, message::market_data_req> market_data_req;
    };

}
}