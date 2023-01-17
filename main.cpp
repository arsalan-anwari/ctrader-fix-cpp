#include <stdio.h>
#include <chrono>
#include <numeric>

#include "data/message_data.hpp"
// #include "data/field_id.hpp"
#include "parser/encode.hpp"
#include "parser/cache.hpp"


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
    using namespace ctrader::parser::cache;

    std::chrono::steady_clock::time_point start_loop, end_encode, end_loop;
    auto clock = std::chrono::nanoseconds{ 546 };
    auto init_dur = std::chrono::nanoseconds{ 100 };
    
    int vals_normal[100] = { 0 };
    int vals_warmed[100] = { 0 };
    
// Test Normal
    for(int i=0; i<100; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        prepare_message<MSG_TYPE::LOGON>(CONN_TYPE::QUOTE);
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_normal[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();
    }

    internal::msg_seq_num = 1;

// Test Warmed
    // pre_fetch_cache at init
    prepare_message<MSG_TYPE::LOGON>(CONN_TYPE::QUOTE);
    prepare_message<MSG_TYPE::LOGON>(CONN_TYPE::QUOTE);
    prepare_message<MSG_TYPE::LOGON>(CONN_TYPE::QUOTE);
    
    internal::msg_seq_num = 1;

    for(int i=0; i<100; i++){
        start_loop = std::chrono::steady_clock::now();
        
        // encode message
        prepare_message<MSG_TYPE::LOGON>(CONN_TYPE::QUOTE);
        end_encode = std::chrono::steady_clock::now();

        // Store encode message time in array
        vals_warmed[i] = std::chrono::duration_cast<std::chrono::nanoseconds> (end_encode - start_loop).count();

        // sync to clock and keep cache hot for next message
        end_loop = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (end_loop - start_loop);
        
        //clock_sync_and_warm_cache<MSG_TYPE::MD_REQ_SUB_DEPTH>( clock, dur, "000000", "1" );
        clock_sync_and_warm_cache<MSG_TYPE::LOGON>( clock, dur, CONN_TYPE::QUOTE );
    }

    // Results
    auto sum_normal = std::accumulate(vals_normal, vals_normal+100, 0);
    auto sum_warmed = std::accumulate(vals_warmed, vals_warmed+100, 0);
    
    printf("sum_normal=%ins\n", (sum_normal/100) );
    printf("sum_warmed=%ins\n", (sum_warmed/100) );

    print_buff(message_data::LOGON);

    return 0;
}