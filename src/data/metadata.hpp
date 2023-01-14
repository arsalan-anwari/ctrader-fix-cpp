#pragma once

#include <string>
#include <cmath> // std::ceil
#include <algorithm> // std::max, std::copy

#include "settings.hpp"
#include "tools/numbers.hpp"
#include "tools/memory.hpp"

namespace ctrader::data::metadata {

namespace internal {

    using namespace ctrader::settings;
    using namespace ctrader::tools;

    consteval int32_t msg_seg_num_digit_size(){
        static_assert(MsgThroughputSec <= 1000, "Setting 'MsgThroughputSec' is invalid! Choose a value between {1 ... 1000}");
        int32_t maxValueSec = ( (24*60*60) * MsgThroughputSec ); // seconds_per_day * msg_throughput_per_sec
        int32_t numDigits = numbers::digit_count(maxValueSec);
        return numDigits;
    };

    consteval uint8_t min_buff_size(){
        // last SOHChar is not added to increase maximum number of digits possible in 32byte chunk. 
        // the more digit you can fit in a single chunk without having to use additional chunk the better runtime performance
        uint8_t metaMinBuffSize = 
            3 + broker_settings::BeginString.size() +  // 8=FIX...|
            3 + 3 + // 9=???...|
            5 + // 35=?|
            4; // 34=?
        return metaMinBuffSize;
    };

    // reserve buffer of 32 bytes or either larger buffer divisible by 32 byte chunks
    consteval uint8_t aligned_chunk_size(uint8_t minBuffSize, int32_t msgSegNumDigitSize) {
        return 
            ( (minBuffSize < 32) && (msgSegNumDigitSize <= ((32 - minBuffSize) + 1)) ) ? 
                32 : (static_cast<uint8_t>(std::ceil( static_cast<float>(minBuffSize)/static_cast<float>(32) ) + static_cast<float>(1)) * 32); 
    };

    consteval memory::chunk_info_t aligned_chunk_padding(uint8_t alignedChunkSize, uint8_t minBuffSize){
        uint8_t buff_start = (alignedChunkSize - minBuffSize) + 1;
        uint8_t offset = buff_start - 1;
        uint8_t count = buff_start <= 18 ? buff_start : 18; 
        return { alignedChunkSize, count, offset };
    };

    constexpr int32_t MsgSegNumDigitSize = msg_seg_num_digit_size();
    constexpr uint8_t MinBuffSize = min_buff_size(); 
    constexpr uint8_t AlignedChunkSize = aligned_chunk_size(MinBuffSize, MsgSegNumDigitSize);
    constexpr memory::chunk_info_t AlignedChunkPadding = aligned_chunk_padding(AlignedChunkSize, MinBuffSize); 

    consteval memory::simple_buffer_t<char, AlignedChunkPadding.size> 
    metadata_from_type( const uint8_t padding, const char msgType, const uint16_t bodyLength, const uint8_t index ){
        memory::simple_buffer_t<char, AlignedChunkPadding.size> buff;

        auto bodylenStr = numbers::to_simple_buffer<3>(bodyLength);

        std::string msgFmt = 
            std::string(index, '\0') +
            "8=" + std::string(broker_settings::BeginString.data()) + SOHChar +
            "9=" + std::string( bodylenStr.data, bodylenStr.data+bodylenStr.size ) + SOHChar +
            "35="+ msgType + SOHChar +
            "34=" + std::string( ((padding - index) + 1), '0' );
        
        std::copy(msgFmt.begin(), msgFmt.end(), buff.data );
        return buff;
    } 

    consteval memory::sparse_chunk_buffer_2d_t<char, AlignedChunkPadding.size, AlignedChunkPadding.offset+1> 
    metadata_lookup_table_from_type( const char msgType, const uint16_t bodyLength) {
        const uint8_t padding_size = AlignedChunkPadding.offset;
        uint8_t prepend_idx = AlignedChunkPadding.offset;

        memory::sparse_chunk_buffer_2d_t<char, AlignedChunkPadding.size, AlignedChunkPadding.offset+1> buffAll;
        
        for(uint8_t i=0; i < padding_size+1; i++){
            auto buff = metadata_from_type(padding_size, msgType, bodyLength, prepend_idx);
            std::copy(buff.data, buff.data+buff.size, buffAll[i] );
            prepend_idx--;
        }

        return buffAll;
    };

} // internal

    constexpr auto MetadataSize = internal::AlignedChunkSize;
    constexpr auto MetadataInfo = internal::AlignedChunkPadding;
    constinit auto MetadataInfoAlias = MetadataInfo;
    constexpr auto MetaDataTemplate = internal::metadata_from_type(MetadataInfo.offset, '0', 111, MetadataInfo.offset);

} // ctrader::data::metadata