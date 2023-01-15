#include <stdio.h>

// #include <chrono>
// #include <algorithm>
// #include "tools/protocol.hpp"

#include "data/message_data.hpp"

// #include "parser/encode.hpp"

// #include "data/field_id.hpp"

int main(void){

    // 126
    // char data[] = "8=FIX.4.4|9=127|35=1|34=000002|52=20230114-23:21:23.995659|49=demo.icmarkets.8536054|56=cServer|57=QUOTE|50=QUOTE|112=SOME_RANDOM_TEXT_TO_TEST|";
    // std::replace(data, data+sizeof(data), '|', '\1');

    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    // uint8_t checksum = ctrader::tools::protocol::calc_checksum<sizeof(data)-1>(data);
    // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // printf("data: %s\n\tchecksum: %i\n\ttime(ns): %i\n", data, checksum, std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());


    using namespace ctrader::data;
    // using namespace ctrader::data::metadata;
    // using namespace ctrader::parser;

    // auto& buffLookup = message_metadata::LOGON;

    // for(uint8_t i=0; i<MetadataInfoAlias.count; i++){
    //     MetadataInfoAlias.select_idx = i;
    //     printf("\n[%i]: %.*s\n", MetadataInfoAlias.select_idx, MetadataInfoAlias.size-MetadataInfoAlias.offset, buffLookup[i]+MetadataInfoAlias.offset);
    //     MetadataInfoAlias.offset--;
    // }

    auto& buff = message_data::LOGON;

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

    // encode::Encoder encoder;
    // encoder.modify_message<encode::MSG_TYPE::LOGON>();

    // printf("\n===AFTER ENCODE===\n");

    // printf("\n[%lu]: %.*s\n", sizeof(buff.data), sizeof(buff.data), buff.data);
    
    // printf("\t[%lu]: %.*s\n", sizeof(buff.header.raw), sizeof(buff.header.raw), buff.header.raw);
    // printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.meta_32a), sizeof(buff.header.meta_32a), buff.header.meta_32a);
    // printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.timestamp_32a), sizeof(buff.header.timestamp_32a), buff.header.timestamp_32a);
    // printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.remainder), sizeof(buff.header.remainder), buff.header.remainder);
    
    // printf("\t[%lu]: %.*s\n", sizeof(buff.body.raw), sizeof(buff.body.raw), buff.body.raw);
    
    // printf("\t[%lu]: %.*s\n", sizeof(buff.checksum), sizeof(buff.checksum), buff.checksum);

    return 0;
}