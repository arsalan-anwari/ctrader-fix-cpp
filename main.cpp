// #include <stdio.h>
// #include <chrono>
// #include <numeric>

// #include "data/message_data.hpp"
// #include "data/field_id.hpp"
// #include "parser/encode.hpp"
// #include "parser/encode_tools.hpp"


// void print_buff(const auto& buff){
//     printf("\n[%lu]: %.*s\n", sizeof(buff.data), sizeof(buff.data), buff.data);
    
//     printf("\t[%lu]: %.*s\n", sizeof(buff.header.raw), sizeof(buff.header.raw), buff.header.raw);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BeginString), sizeof(buff.header.field.BeginString), buff.header.field.BeginString);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BodyLength), sizeof(buff.header.field.BodyLength), buff.header.field.BodyLength);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgType), sizeof(buff.header.field.MsgType), buff.header.field.MsgType);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgSeqNum), sizeof(buff.header.field.MsgSeqNum), buff.header.field.MsgSeqNum);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.timestamp_32a), sizeof(buff.header.field.timestamp_32a), buff.header.field.timestamp_32a);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.SenderCompID), sizeof(buff.header.field.SenderCompID), buff.header.field.SenderCompID);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.TargetCompID), sizeof(buff.header.field.TargetCompID), buff.header.field.TargetCompID);
//     printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.TargetSubID), sizeof(buff.header.field.TargetSubID), buff.header.field.TargetSubID);
    
//     printf("\t[%lu]: %.*s\n", sizeof(buff.body.raw), sizeof(buff.body.raw), buff.body.raw);
    
//     printf("\t[%lu]: %.*s\n", sizeof(buff.checksum), sizeof(buff.checksum), buff.checksum);
// }


// int main(void){

//     using namespace ctrader::data;
//     using namespace ctrader::parser::encode;
//     using namespace ctrader::parser::encode_tools;
//     using namespace ctrader::types::message;

// // // Sanity Check
// //     printf("\n====== BEFORE ENCODE =====\n");
// //     print_buff(message_data::quote::MD_REQ_SUB_DEPTH);

// //     printf("\n====== AFTER ENCODE =====\n");
// //     Encoder<CONN::QUOTE> encoder;
// //     encoder.encode_message<MSG::MD_REQ_SUB_DEPTH>( { field_id::Keys[0], 242243 } );
// //     print_buff(message_data::quote::MD_REQ_SUB_DEPTH);
// //     printf("field_id::Keys[0]=%s\n", field_id::Keys[0]);

// // Initialiaze data for tests

//     // // Setting 1
//     auto& MSG_DATA_TO_TEST = message_data::quote::MD_REQ_SUB_DEPTH;
//     constexpr MSG MSG_TYPE_TO_TEST = MSG::MD_REQ_SUB_DEPTH;
//     constexpr CONN CONN_TYPE_TO_TEST = CONN::QUOTE;
//     #define MSG_TYPE_PARAMS_TO_TEST(i) SYMBOL::EUR_USD, field_id::Keys[i]
//     constexpr int MSG_LOOP_COUNT = 5;

//     // Setting 2
//     // auto& MSG_DATA_TO_TEST = message_data::quote::LOGON;
//     // constexpr MSG MSG_TYPE_TO_TEST = MSG::LOGON;
//     // constexpr CONN CONN_TYPE_TO_TEST = CONN::QUOTE;
//     // constexpr int MSG_LOOP_COUNT = 5;
//     // #define MSG_TYPE_PARAMS_TO_TEST(LOOP_ITERATION) { }

//     ClockSync<CONN_TYPE_TO_TEST> clock(500U);
//     Encoder<CONN_TYPE_TO_TEST> encoder;

//     std::chrono::steady_clock::time_point start_loop, end_encode, end_loop;
//     int vals_normal[MSG_LOOP_COUNT] = { 0 };
//     int vals_warmed[MSG_LOOP_COUNT] = { 0 };

//     printf("\n====== BEFORE ENCODE =====\n");
//     print_buff(MSG_DATA_TO_TEST);

// // Test Normal
//     for(int i=0; i<MSG_LOOP_COUNT; i++){
//         start_loop = std::chrono::steady_clock::now();
        
//         // encode message
//         encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(i));
//         end_encode = std::chrono::steady_clock::now();

//         // Store encode message time in array
//         vals_normal[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();
//     }

//     encoder.reset_seq_num();

// // Test Warmed

//     // pre_fetch_cache at init
//     encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
//     encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
//     encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
    
//     encoder.reset_seq_num();

//     for(int i=0; i<MSG_LOOP_COUNT; i++){
//         start_loop = std::chrono::steady_clock::now();
        
//         // encode message
//         encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(i));
//         end_encode = std::chrono::steady_clock::now();

//         // Store encode message time in array
//         vals_warmed[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();

//         // sync to clock and keep cache hot for next message
//         end_loop = std::chrono::steady_clock::now();
//         auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (end_loop - start_loop);
        
//         clock.sync_and_warm_cache<MSG_TYPE_TO_TEST>(dur);
//     }

//     printf("\n====== AFTER ENCODE =====\n");
//     encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
//     print_buff(MSG_DATA_TO_TEST);
    

// // Results
//     printf("\n====== RESULTS =====\n");
//     auto sum_normal = std::accumulate(vals_normal, vals_normal+MSG_LOOP_COUNT, 0);
//     auto sum_warmed = std::accumulate(vals_warmed, vals_warmed+MSG_LOOP_COUNT, 0);
//     printf("\t- MSG_LOOP_COUNT=%u\n", MSG_LOOP_COUNT );
//     printf("\t- sum_normal=%uns\n", (sum_normal/MSG_LOOP_COUNT) );
//     printf("\t- sum_warmed=%uns\n", (sum_warmed/MSG_LOOP_COUNT) );

//     return 0;
// }



#include <stdio.h>
#include <stdint.h>

#include "parser/decode.hpp"

const char* data = 
"8=FIX.4.4|9=1128"
"|35=X|34=3|49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=demo.icmarkets.8536054"
"|268=25"
"|279=0|269=1|278=2291667248|55=1|270=1.08754|271=5000000"
"|279=0|269=1|278=2291667249|55=1|270=1.08757|271=10000000"
"|279=0|269=1|278=2291667250|55=1|270=1.08752|271=1500000"
"|279=0|269=1|278=2291667252|55=1|270=1.08749|271=100000"
"|279=0|269=1|278=2291667253|55=1|270=1.08753|271=3000000"
"|279=0|269=1|278=2291667254|55=1|270=1.08748|271=50000"
"|279=0|269=1|278=2291667255|55=1|270=1.08764|271=20000000"
"|279=2|278=2291666392|55=1"
"|279=2|278=2291666393|55=1"
"|279=2|278=2291666394|55=1"
"|279=2|278=2291661810|55=1"
"|279=2|278=2291666388|55=1"
"|279=2|278=2291666389|55=1"
"|279=2|278=2291666390|55=1"
"|279=0|269=0|278=2291667242|55=1|270=1.08747|271=1500000"
"|279=0|269=0|278=2291667243|55=1|270=1.08742|271=10000000"
"|279=0|269=0|278=2291667244|55=1|270=1.08748|271=150000"
"|279=0|269=0|278=2291667245|55=1|270=1.08745|271=3000000"
"|279=0|269=0|278=2291667246|55=1|270=1.08744|271=5000000"
"|279=0|269=0|278=2291667247|55=1|270=1.08737|271=20000000"
"|279=2|278=2291666381|55=1"
"|279=2|278=2291666384|55=1"
"|279=2|278=2291666385|55=1"
"|279=2|278=2291666386|55=1"
"|279=2|278=2291666387|55=1"
"|10=244|";

int main(){

    using namespace ctrader::parser::decode;
    // uint16_t absolute_offset = 153;
    // uint8_t found_idx = 5;
    // uint16_t absolute_offset_lookup[2] = {0, absolute_offset+found_idx-1};
    // uint16_t absolute_offset_lookup_idx = 
    // static_cast<uint8_t>(
    //         ((absolute_offset - found_idx) < absolute_offset) && ( (absolute_offset + found_idx) - 1 )
    // );

    // printf("%u\n", absolute_offset_lookup[absolute_offset_lookup_idx] );

    Decoder<DECODE_TYPE::MARKET_DATA_INCREMENTAL> decoder;



    // //printf("%u", decoder.decode(data));
    // printf("%s", DECODE_TYPE_PATTERN_LOOKUP[1] );

    return 0;
}