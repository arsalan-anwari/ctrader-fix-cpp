#pragma once

#include <tuple>
#include <string_view>

#include "types/numbers.hpp"
#include "decoder/decode_method.hpp"

#include "types/concepts.hpp"

namespace ctrader {
namespace decode {

    template<u8 MaxEntries> requires concepts::is_power_of_2<MaxEntries>
    class decoder {
    public:

        template<response T = response::undefined>
        std::tuple<response, u8> decode(const char* msg) {
            const auto [body_length, body_offset] = get_body_meta(msg);
        
            if constexpr (T == response::market_data_incremental) {
                u8 num_entries = market_data_incremental.decode(std::string_view(msg+body_offset, body_length));
                return { response::market_data_incremental, num_entries };
            }
            else {
                return { response::undefined, 0U };
            }
        }

        template<response R>
        std::array<response_data<R>, MaxEntries> const& get_response() const {
            if constexpr (R == response::market_data_incremental) {
                return market_data_incremental.market_data;
            }
        }

    private:
        std::tuple<u16, u16> get_body_meta(const char* msg) {
            // example: 8=FIX.4.4|9=1140|35=X|34=3|49=cServer|50=QUOTE|52=20230124-13:30:46.130|56=demo.icmarkets.8536054|262=b3j2b26|268=25

            const i32 message_size_digit_size = find<6U>(msg + 12U, settings::SOH_CHAR);
            const u32 message_size = to_unsigned_integral<u32>(msg + 12U, message_size_digit_size);

            const u32 msg_seq_num_offset = 12U + message_size_digit_size + 9U; // 8=FIX.4.4|9=1128|35=X|34=
            const i32 msg_seq_num_digit_size = find<20U>(msg + msg_seq_num_offset, settings::SOH_CHAR);
            const u32 msg_seq_num = to_unsigned_integral<u32>(msg + msg_seq_num_offset, msg_seq_num_digit_size);

            const u32 body_offset = (msg_seq_num_offset + msg_seq_num_digit_size + 49U + static_cast<u32>(settings::broker::SENDER_COMP_ID.size()));

            return { static_cast<u16>(message_size - (body_offset - (12U + message_size_digit_size)) - 9U), static_cast<u16>(body_offset) };
        }

        decode_method<MaxEntries, response::market_data_incremental> market_data_incremental;

    };

}
}