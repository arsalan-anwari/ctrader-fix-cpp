#pragma once

#include "encode.hpp"
#include "body.hpp"
#include "header.hpp"

namespace ctrader::types::message {

    using namespace ctrader::types::encode; 
    using namespace ctrader::types::numbers; 
    using namespace ctrader::types::body; 

    template<ENCODE_TYPE T>
    struct message_t {
        union{
            char data[sizeof(header_t) + sizeof(body_t<T>) + 8];
            struct {
                header_t header;
                body_t<T> body;
                char checksum[8];
            };
        };
    };

} // ctrader::types::message
