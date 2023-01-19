#pragma once

#include "header.hpp"
#include "message_type.hpp"

namespace ctrader::data::message {

    using MSG = ctrader::data::message_type::MSG;

    template<MSG T>
    struct message_t {
        union{
            char data[sizeof(ctrader::data::header::header_t) + sizeof(ctrader::data::message_type::internal::body_t<T>) + 8];
            struct {
                ctrader::data::header::header_t header;
                ctrader::data::message_type::internal::body_t<T> body;
                char checksum[8];
            };
        };
    };

} // ctrader::data::message
