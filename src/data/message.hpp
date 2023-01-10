#pragma once

#include "header.hpp"
#include "message_type.hpp"

namespace ctrader::data::message {

    using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;

    template<MSG_TYPE T>
    struct message_t {
        union{
            char data[sizeof(ctrader::data::header::header_t) + sizeof(ctrader::data::message_type::internal::body_t<T>) + 8];
            struct {
                ctrader::data::header::header_t header;
                ctrader::data::message_type::internal::body_t<T> body;
                char cheksum[8];
            };
        };
    };

} // ctrader::data::message
