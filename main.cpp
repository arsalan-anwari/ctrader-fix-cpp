#include <stdio.h>
#include <chrono>
#include <numeric>

#include "data/message_data.hpp"
#include "data/field_id.hpp"
#include "parser/encode.hpp"
#include "parser/encode_tools.hpp"


void print_buff(const auto& buff){
    printf("\n[%lu]: %.*s\n", sizeof(buff.data), sizeof(buff.data), buff.data);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.header.raw), sizeof(buff.header.raw), buff.header.raw);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BeginString), sizeof(buff.header.field.BeginString), buff.header.field.BeginString);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BodyLength), sizeof(buff.header.field.BodyLength), buff.header.field.BodyLength);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgType), sizeof(buff.header.field.MsgType), buff.header.field.MsgType);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgSeqNum), sizeof(buff.header.field.MsgSeqNum), buff.header.field.MsgSeqNum);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.timestamp_32a), sizeof(buff.header.field.timestamp_32a), buff.header.field.timestamp_32a);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.SenderCompID), sizeof(buff.header.field.SenderCompID), buff.header.field.SenderCompID);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.TargetCompID), sizeof(buff.header.field.TargetCompID), buff.header.field.TargetCompID);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.TargetSubID), sizeof(buff.header.field.TargetSubID), buff.header.field.TargetSubID);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.body.raw), sizeof(buff.body.raw), buff.body.raw);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.checksum), sizeof(buff.checksum), buff.checksum);
}


int main(void){

    using namespace ctrader::data;
    using namespace ctrader::parser::encode;
    using namespace ctrader::parser::encode_tools;

// Sanity Check
    // printf("\n====== BEFORE ENCODE =====\n");
    // print_buff(message_data::quote::MD_REQ_SUB_DEPTH);

    // uint64_t symbol = 323;
    // printf("\n====== AFTER ENCODE =====\n");
    // Encoder<CONN::QUOTE> encoder;
    // encoder.encode_message<MSG::MD_REQ_SUB_DEPTH>("ddsfsa", symbol);
    // print_buff(message_data::quote::MD_REQ_SUB_DEPTH);

// Initialiaze data for tests
    //auto symbol = static_cast<uint64_t>(723894UL);

    auto& MSG_DATA_TO_TEST = message_data::quote::MD_REQ_SUB_DEPTH;
    constexpr MSG MSG_TYPE_TO_TEST = MSG::MD_REQ_SUB_DEPTH;
    constexpr CONN CONN_TYPE_TO_TEST = CONN::QUOTE;
    #define MSG_TYPE_PARAMS_TO_TEST(i) field_id::Keys[i], 242243
    constexpr int MSG_LOOP_COUNT = 5;

    ClockSync<CONN_TYPE_TO_TEST> clock(500U);
    Encoder<CONN_TYPE_TO_TEST> encoder;

    std::chrono::steady_clock::time_point start_loop, end_encode, end_loop;
    int vals_normal[MSG_LOOP_COUNT] = { 0 };
    int vals_warmed[MSG_LOOP_COUNT] = { 0 };

    printf("\n====== BEFORE ENCODE =====\n");
    print_buff(MSG_DATA_TO_TEST);

// Test Normal
    for(int i=0; i<MSG_LOOP_COUNT; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(i));
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_normal[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();
    }

    encoder.reset_seq_num();

// Test Warmed

    // pre_fetch_cache at init
    encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
    encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
    encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
    
    encoder.reset_seq_num();

    for(int i=0; i<MSG_LOOP_COUNT; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(i));
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_warmed[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();

        // sync to clock and keep cache hot for next message
        end_loop = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (end_loop - start_loop);
        
        clock.sync_and_warm_cache<MSG_TYPE_TO_TEST>(dur);
    }

    printf("\n====== AFTER ENCODE =====\n");
    encoder.encode_message<MSG_TYPE_TO_TEST>(MSG_TYPE_PARAMS_TO_TEST(0));
    print_buff(MSG_DATA_TO_TEST);
    

// Results
    printf("\n====== RESULTS =====\n");
    auto sum_normal = std::accumulate(vals_normal, vals_normal+MSG_LOOP_COUNT, 0);
    auto sum_warmed = std::accumulate(vals_warmed, vals_warmed+MSG_LOOP_COUNT, 0);
    printf("\t- MSG_LOOP_COUNT=%u\n", MSG_LOOP_COUNT );
    printf("\t- sum_normal=%uns\n", (sum_normal/MSG_LOOP_COUNT) );
    printf("\t- sum_warmed=%uns\n", (sum_warmed/MSG_LOOP_COUNT) );

    return 0;
}