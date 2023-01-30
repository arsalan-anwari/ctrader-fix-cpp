#pragma once

#include <cstring>
#include <string>

#include "types/decode.hpp"
#include "types/memory.hpp"

#include "tools/concepts.hpp"
#include "tools/numbers.hpp"

#include "global.hpp"
#include "settings.hpp"

namespace ctrader::parser::decode {

using namespace ctrader::types::memory;
using namespace ctrader::types::decode;
using namespace ctrader::tools::concepts;

template<DECODE_TYPE T>
using message_container = simple_buffer_t<decode_data<T>, 256>;

namespace internal {

    using namespace ctrader::tools::numbers;
    using namespace ctrader::data;
    using namespace ctrader::settings;
    using namespace ctrader::parser::global;

    inline __attribute__((always_inline))
    uint8_t find_pattern_32a(const char* text, const char* pattern){
        std::string _text(text, 32);
        std::string _pattern(pattern, 4);
        auto pos = _text.find(_pattern);
        if (pos == std::string::npos){ return 0; }
        return static_cast<uint8_t> (pos + 1);
    };
    
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    decode_metadata get_decode_meta(const char* data){
        char msg_size_buff[4];
        const int8_t msg_digit_size_lookup[2] = {3, 4};

        std::memcpy(msg_size_buff, data+12, 4);
        int8_t msg_digit_last_val = static_cast<int8_t>(msg_size_buff[3]) - 57;
        int8_t msg_digit_size = msg_digit_size_lookup[
            static_cast<uint8_t>( (msg_digit_last_val > -10) && (msg_digit_last_val <= 0) )
        ];

        uint16_t multiplier = 1;
        uint16_t total_msg_size = 0;
        for(uint8_t i = msg_digit_size; i > 0; i--){
            total_msg_size += (msg_size_buff[i-1] - '0') * multiplier;
            multiplier *= 10;
        }
        
        const uint16_t header_size = ( 
            9 + // |35=X|34=
            msg_seq_num_digit_size + // {0:1-18}
            49 + // |49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=
            broker_settings::SenderCompID.size() // demo.icmarkets.8536054
        );

        const uint16_t size = total_msg_size - header_size;

        const uint16_t offset = (
            12 + // 8=FIX.4.4|9=
            msg_digit_size + // {0:1-4}
            header_size 
        );
        
        
        return decode_metadata{ offset, size };
    };

    template<DECODE_TYPE T>
    inline __attribute__((always_inline)) __attribute__((optimize("unroll-loops")))
    void parsing_algorithm( message_container<T>& parsed_messages, const char* data, uint16_t size ){};

    template<> void parsing_algorithm<DECODE_TYPE::MARKET_DATA_INCREMENTAL>
    ( message_container<DECODE_TYPE::MARKET_DATA_INCREMENTAL>& parsed_messages, const char* data, uint16_t size ){
        const uint8_t skip_size_lookup[3] = {50, 4, 26};

        sparse_chunk_buffer_2d_t<uint16_t, 256, 2> found_message_idx_buff;
        const uint8_t found_message_idx_buff_lookup[3] = {0, 1, 1};
        uint8_t found_message_idx_buff_sizes[2] = { 0 };

        // const char* action_new_ptr = found_message_idx_buff[0];
        // const char* action_delete_ptr = found_message_idx_buff[1];
        
        uint8_t update_translation = static_cast<uint8_t>(data[5] - '0');
        uint8_t idx_buff_translation = found_message_idx_buff_lookup[ update_translation ]; 
        
        found_message_idx[ idx_buff_translation ][0] = 0;
        found_message_idx_buff_sizes [ idx_buff_translation ]++; 

        uint16_t absolute_offset = skip_size_lookup[update_translation];

        uint8_t found_idx, found_state, buff_element_num, buff_type;
        uint16_t idx_buff_item_insert_location, new_absolute_offset;

        // 'Vectorized' loop
        while ( (absolute_offset + 32) < size ){
            // Find pattern in chunk: n=0 -> not found n>0 -> found
            //  if not found: set temporary offset value to 0
            //  if found: calculate new temporary offset value
            found_idx = find_pattern_32a(data+absolute_offset, DECODE_TYPE_PATTERN_LOOKUP[1]);
            found_state = static_cast<uint8_t>( found_idx > 0 );

            uint16_t absolute_offset_lookup[2] = {0, absolute_offset+found_idx-1};
            new_absolute_offset = absolute_offset_lookup[found_state];

            // Find which buffer and get last insert index of that buffer
            update_translation = static_cast<uint8_t>(data[new_absolute_offset+5] - '0');
            buff_type = found_message_idx_buff_lookup[ update_translation ];
            buff_element_num = found_message_idx_buff_sizes [ buff_type ];

            // These lookups are used to handle the if-else logic when 'found_idx' is 0
            uint8_t new_buff_sizes[2] = { buff_element_num, buff_element_num+1 };
            uint8_t buff_element_nums[2] = { 0, buff_element_num+1 };

            // Calculate the new insert index of the correct buffer to insert new_absolute_offset
            //  if not found: overwrite value at position [buff_type][0] and overwrite buffer[buff_type] size with old buffer size
            //  if found: insert element at position [buff_type][buff_element_num] and increment buffer[buff_type] size by one.
            buff_element_num = buff_element_nums[ found_state ];
            found_message_idx[ buff_type ][ buff_element_num ] = new_absolute_offset;
            found_message_idx_buff_sizes [ buff_type ] = new_buff_sizes[ found_state ];

            uint8_t new_skip_size_lookup[2] = { skip_size_lookup[1], skip_size_lookup[update_translation] };
            absolute_offset += new_skip_size_lookup [ found_state ];

        }


    };

} // internal


template <DECODE_TYPE T>
struct Decoder{
    Decoder(){};

    inline __attribute__((always_inline))
    uint8_t decode(const char* data){ 
        decode_metadata meta = internal::get_decode_meta(data);

        internal::parsing_algorithm<T>(parsed_messages, data+meta.offset, meta.size); 
        return parsed_messages.real_size; 
        // return meta.size;
    };

private:
    message_container<T> parsed_messages;
};


} // ctrader::parser

// /**
// Received:  8=FIX.4.4|9=633
// |35=W|34=2|49=cServer|50=QUOTE|52=20230124-13:30:46.025|56=demo.icmarkets.8536054
// |55=1
// |268=12
// |269=1|270=1.08755|271=1500000|278=2291666392
// |269=1|270=1.08765|271=20000000|278=2291666393
// |269=1|270=1.08753|271=100000|278=2291666394
// |269=1|270=1.08759|271=10000000|278=2291661810
// |269=1|270=1.08752|271=50000|278=2291666388
// |269=1|270=1.08757|271=5000000|278=2291666389
// |269=1|270=1.08756|271=3000000|278=2291666390
// |269=0|270=1.08747|271=10000000|278=2291666384
// |269=0|270=1.08741|271=20000000|278=2291666385
// |269=0|270=1.0875x|271=3000000|278=2291666386
// |269=0|270=1.08752|271=1650000|278=2291666387
// |269=0|270=1.08749|271=5000000|278=2291666381
// |10=091|

// Received:  
// 8=FIX.4.4|9=1128
// |35=X|34=3|49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=demo.icmarkets.8536054
// |268=25
// |279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000
// |279=0|269=1|278=2291667249|55=1|270=1.08757|271=10000000
// |279=0|269=1|278=2291667250|55=1|270=1.08752|271=1500000
// |279=0|269=1|278=2291667252|55=1|270=1.08749|271=100000
// |279=0|269=1|278=2291667253|55=1|270=1.08753|271=3000000
// |279=0|269=1|278=2291667254|55=1|270=1.08748|271=50000
// |279=0|269=1|278=2291667255|55=1|270=1.08764|271=20000000
// |279=2|278=2291666392|55=1
// |279=2|278=2291666393|55=1
// |279=2|278=2291666394|55=1
// |279=2|278=2291661810|55=1
// |279=2|278=2291666388|55=1
// |279=2|278=2291666389|55=1
// |279=2|278=2291666390|55=1
// |279=0|269=0|278=2291667242|55=1|270=1.08747|271=1500000
// |279=0|269=0|278=2291667243|55=1|270=1.08742|271=10000000
// |279=0|269=0|278=2291667244|55=1|270=1.08748|271=150000
// |279=0|269=0|278=2291667245|55=1|270=1.08745|271=3000000
// |279=0|269=0|278=2291667246|55=1|270=1.08744|271=5000000
// |279=0|269=0|278=2291667247|55=1|270=1.08737|271=20000000
// |279=2|278=2291666381|55=1
// |279=2|278=2291666384|55=1
// |279=2|278=2291666385|55=1
// |279=2|278=2291666386|55=1
// |279=2|278=2291666387|55=1
// |10=244|

// 8=FIX.4.4|9=855|35=X|34=4|49=cServer|50=QUOTE|52=20230124-13:30:46.692|56=demo.icmarkets.8536054
// |268=18
// |279=0|269=1|278=2291668712|55=1|270=1.08754|271=3000000
// |279=0|269=1|278=2291668713|55=1|270=1.08755|271=5000000
// |279=0|269=1|278=2291668705|55=1|270=1.08772|271=50000000
// |279=0|269=1|278=2291668706|55=1|270=1.08753|271=1500000
// |279=0|269=1|278=2291668707|55=1|270=1.08758|271=10000000
// |279=0|269=1|278=2291668709|55=1|270=1.08749|271=50000
// |279=0|269=1|278=2291668711|55=1|270=1.0875|271=100000
// |279=2|278=2291667248|55=1
// |279=2|278=2291667249|55=1
// |279=2|278=2291667250|55=1
// |279=2|278=2291667252|55=1
// |279=2|278=2291667253|55=1
// |279=2|278=2291667254|55=1
// |279=0|269=0|278=2291668699|55=1|270=1.08749|271=150000
// |279=0|269=0|278=2291668700|55=1|270=1.08727|271=50000000
// |279=0|269=0|278=2291668703|55=1|270=1.08736|271=20000000
// |279=2|278=2291667244|55=1|279=2|278=2291667247|55=1
// |10=105|
// */