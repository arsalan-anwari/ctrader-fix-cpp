#pragma once


#include "types/numbers.hpp"
#include "types/message.hpp"
#include "types/encode.hpp"

#include "encoder/encode_method.hpp"
#include "encode_settings.hpp"
#include "tools/bitwise.hpp"


namespace ctrader {
namespace encode {

template<connection Connection, encode_options Settings = default_encode_settings >
    class encoder {
    public:

        template<request Request, typename... Field>
        void encode(Field... fields) {
            if constexpr (Request == request::heart_beat) { heart_beat.prepare(msg_seq_num, fields...); }
            if constexpr (Request == request::test_req) { test_req.prepare(msg_seq_num, fields...); }
            if constexpr (Request == request::logon) { logon.prepare(msg_seq_num, fields...); }
            if constexpr (Request == request::market_data_req) { market_data_req.prepare(msg_seq_num, fields...); }
            advance_seq_num();
        }

        inline void advance_seq_num() { msg_seq_num++; }
        inline void reduce_seq_num() { msg_seq_num -= bitwise::gte(msg_seq_num, 1); }
        inline void reset_seq_num() { msg_seq_num = 1; }
        inline u64 get_seq_num() const { return msg_seq_num; }

        template<request Request>
        packet_t<Settings, Request> const& get_buffer() const {
            if constexpr (Request == request::heart_beat) { return heart_beat.buff; }
            if constexpr (Request == request::test_req) { return test_req.buff; }
            if constexpr (Request == request::logon) { return logon.buff; }
            if constexpr (Request == request::market_data_req) { return market_data_req.buff; }
        }

    private:
        u64 msg_seq_num = 1u;

        encode_method<Connection, Settings, request::heart_beat> heart_beat;
        encode_method<Connection, Settings, request::test_req> test_req;
        encode_method<Connection, Settings, request::logon> logon;
        encode_method<Connection, Settings, request::market_data_req> market_data_req;
    };

}
}