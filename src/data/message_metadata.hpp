#pragma once

#include "message_type.hpp"
#include "metadata.hpp"

namespace ctrader::data::message_metadata{
    using MSG_TYPE = ctrader::data::message_type::MSG_TYPE;

    namespace internal{
        using namespace ctrader::data;
        using namespace ctrader::tools;
        
        template<MSG_TYPE T>
        consteval memory::sparse_chunk_buffer_2d_t<char, metadata::MetadataInfo.size, metadata::MetadataInfo.offset+1>
        message_metadata_from_type(){
            message_type::internal::body_t<T> body;
            const char msgType = ctrader::data::message_type::internal::MSG_TYPE_LOOKUP[static_cast<uint8_t>(T)];
            const uint16_t bodylen = ( message_type::BodyLengthHeaderPart + sizeof(body.data));
            return metadata::internal::metadata_lookup_table_from_type(msgType, bodylen);
        };
    }

    constexpr auto buffLookup = internal::message_metadata_from_type<MSG_TYPE::MD_REQ_SUB_DEPTH>();
    

}