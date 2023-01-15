#include <stdio.h>
#include <chrono>

#include "data/message_data.hpp"
#include "parser/encode.hpp"


void print_buff(const auto& buff){
    printf("\n[%lu]: %.*s\n", sizeof(buff.data), sizeof(buff.data), buff.data);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.header.raw), sizeof(buff.header.raw), buff.header.raw);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BeginString), sizeof(buff.header.field.BeginString), buff.header.field.BeginString);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.BodyLength), sizeof(buff.header.field.BodyLength), buff.header.field.BodyLength);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgType), sizeof(buff.header.field.MsgType), buff.header.field.MsgType);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.MsgSeqNum), sizeof(buff.header.field.MsgSeqNum), buff.header.field.MsgSeqNum);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.timestamp_32a), sizeof(buff.header.field.timestamp_32a), buff.header.field.timestamp_32a);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.field.remainder), sizeof(buff.header.field.remainder), buff.header.field.remainder);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.body.raw), sizeof(buff.body.raw), buff.body.raw);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.checksum), sizeof(buff.checksum), buff.checksum);
}


int main(void){

    
    using namespace ctrader::data;
    using namespace ctrader::parser;
    std::chrono::steady_clock::time_point begin, end;
    encode_message<MSG_TYPE::LOGON>();

    // main loop
    int i;
    for(i=0; i<100; i++){

        begin = std::chrono::steady_clock::now();
        encode_message<MSG_TYPE::LOGON>();
        end = std::chrono::steady_clock::now();

        printf("%i ", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());

        begin = std::chrono::steady_clock::now();
        encode_message<MSG_TYPE::TEST_REQ>();
        end = std::chrono::steady_clock::now();

        printf("%i\n", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());

    }

    printf("\nRUN %i:\n", i);

    return 0;
}