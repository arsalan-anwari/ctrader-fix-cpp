#include <stdio.h>
#include <chrono>
#include <numeric>

#include "data/message_data.hpp"
// #include "data/field_id.hpp"
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

// Initialiaze data for tests
    using namespace ctrader::data;
    using namespace ctrader::parser::encode;
    using namespace ctrader::parser::encode_tools;

    ClockSync<CONN::QUOTE> quote_clock(500U);
    // ClockSync<CONN::TRADE> trade_clock(500U);

    Encoder<CONN::QUOTE> quote_encoder;
    // Encoder<CONN::TRADE> trade_encoder;

    std::chrono::steady_clock::time_point start_loop, end_encode, end_loop;
    constexpr int loop_count = 10;
    int vals_normal[loop_count] = { 0 };
    int vals_warmed[loop_count] = { 0 };

    printf("\n====== BEFORE ENCODE =====\n");
    print_buff(message_data::quote::LOGON);
    // print_buff(message_data::trade::LOGON);


    // quote_encoder.encode_message<MSG::LOGON>();
    // trade_encoder.encode_message<MSG::LOGON>();

// Test Normal
    for(int i=0; i<loop_count; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        quote_encoder.encode_message<MSG::LOGON>();
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_normal[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();
    }

    quote_encoder.reset_seq_num();

// Test Warmed

    // pre_fetch_cache at init
    quote_encoder.encode_message<MSG::LOGON>();
    quote_encoder.encode_message<MSG::LOGON>();
    quote_encoder.encode_message<MSG::LOGON>();
    
    quote_encoder.reset_seq_num();

    for(int i=0; i<loop_count; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        quote_encoder.encode_message<MSG::LOGON>();
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_warmed[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();

        // sync to clock and keep cache hot for next message
        end_loop = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (end_loop - start_loop);
        
        quote_clock.sync_and_warm_cache<MSG::LOGON>(dur);
    }

    printf("\n====== AFTER ENCODE =====\n");
    print_buff(message_data::quote::LOGON);
    // print_buff(message_data::trade::LOGON);

// Results
    printf("\n====== RESULTS =====\n");
    auto sum_normal = std::accumulate(vals_normal, vals_normal+loop_count, 0);
    auto sum_warmed = std::accumulate(vals_warmed, vals_warmed+loop_count, 0);
    printf("\t- loop_count=%i\n", loop_count );
    printf("\t- sum_normal=%ins\n", (sum_normal/loop_count) );
    printf("\t- sum_warmed=%ins\n", (sum_warmed/loop_count) );

    return 0;
}