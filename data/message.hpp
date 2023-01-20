#pragma once

#include "types/message_type.hpp"
#include "body.hpp"
#include "header.hpp"

namespace ctrader::data::message {

    template<ctrader::types::message_type::MSG T>
    struct message_t {
        union{
            char data[sizeof(ctrader::data::header::header_t) + sizeof(ctrader::data::body::internal::body_t<T>) + 8];
            struct {
                ctrader::data::header::header_t header;
                ctrader::data::body::internal::body_t<T> body;
                char checksum[8];
            };
        };
    };

} // ctrader::data::message
