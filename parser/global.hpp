#pragma once

namespace ctrader::parser::global {

    struct MsgSeqNum {
        static int64_t msg_seq_num;
        static int64_t msg_seq_num_base;
        static uint16_t msg_seq_num_digit_size;
    };

    int64_t MsgSeqNum::msg_seq_num = 1;
    int64_t MsgSeqNum::msg_seq_num_base = 10;
    uint16_t MsgSeqNum::msg_seq_num_digit_size = 1;

}