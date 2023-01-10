#include <stdio.h>

// #include <chrono>
// #include "tools/protocol.hpp"

// #include "data/message_metadata.hpp"

#include "data/message_data.hpp"

// #include "data/id_lookup.hpp"

int main(void){

    // const char data[] = "8=FIX.4.4|9=176|35=V|34=0|52=00000000-00:00:00.000000|49=demo.icmarkets.8536054|56=cServer|57=QUOTE|50=QUOTE|262=000000|263=1|264=0|265=1|267=2|269=0|269=1|146=1|55=1|";

    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    // uint8_t checksum = ctrader::tools::protocol::calc_checksum<sizeof(data)-1>(data);
    // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // printf("data: %s\n\tchecksum: %i\n\ttime(ns): %i\n", data, checksum, std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());


    // using namespace ctrader::data::message_metadata;
    // using namespace ctrader::data::metadata;

    // for(uint8_t i=0; i<MetadataInfoAlias.count; i++){
    //     MetadataInfoAlias.select_idx = i;
    //     printf("\n[%i]: %.*s\n", MetadataInfoAlias.select_idx, MetadataInfoAlias.size-MetadataInfoAlias.offset, buffLookup[i]+MetadataInfoAlias.offset);
    //     MetadataInfoAlias.offset--;
    // }


    using namespace ctrader::data::message_data;
    auto& buff = MD_REQ_SUB_DEPTH;

    printf("\n[%lu]: %.*s\n", sizeof(buff.data), sizeof(buff.data), buff.data);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.header.data), sizeof(buff.header.data), buff.header.data);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.meta_32a), sizeof(buff.header.meta_32a), buff.header.meta_32a);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.timestamp_32a), sizeof(buff.header.timestamp_32a), buff.header.timestamp_32a);
    printf("\t\t[%lu]: %.*s\n", sizeof(buff.header.remainder), sizeof(buff.header.remainder), buff.header.remainder);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.body.data), sizeof(buff.body.data), buff.body.data);
    
    printf("\t[%lu]: %.*s\n", sizeof(buff.cheksum), sizeof(buff.cheksum), buff.cheksum);


    // using namespace ctrader::data::id_lookup;
    // printf("MDReqIDKey=%s", MDReqIDKey);

    

    return 0;
}