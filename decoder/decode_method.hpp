#pragma once

#include <iostream>
#include <array>
#include <string_view>
#include <format>

#include "../types/numbers.hpp"
#include "../types/message.hpp"
#include "../types/decode.hpp"
#include "../tools/bitwise.hpp"
#include "../tools/find.hpp"
#include "../tools/convert.hpp"
#include "../settings.hpp"



namespace ctrader {
namespace decode{

    template<u8 BufferSize, response R>
    struct decode_method {};

    template<u8 BufferSize>
    struct decode_method<BufferSize, response::market_data_incremental> {
        std::array<response_data<response::market_data_incremental>, BufferSize> market_data;
    
        u8 decode(std::string_view msg) {
            // Find the correct chunk_size depending on available cpu extentions or external hardware
            // Set correct array index offset for md_action::remove entries.
            static constexpr u32 stride = get_maximum_stride_epi8();
            static constexpr u32 remove_action_offset = static_cast<u32>(BufferSize);

            // Search for beginning of 'NoMDEntries' tag and get number of entries.
            // Offset message by 'NoMDEntries' to start from first 'MDUpdateAction' tag. 
            const i32 pos = find<19U, 4U>(msg.data(), std::format("{}268", settings::SOH).c_str() );
            msg = msg.substr(pos, msg.size()-pos);
            const auto [num_entries, entry_digit_size] = get_num_entries(msg.substr(5, 4));

            // Move request To first occurance of md_action tag '|279='. 
            msg = msg.substr(5 + entry_digit_size, msg.size() - (5 + entry_digit_size));

            // Find which symbol this message resolves to, to determine correct skip_sizes for the hardware optimized
            // pattern searching routine. The size of this first chunk is unknown so a regular scalar operation needs
            // be to be performed to find with occurance of 'Symbol' tag. 
            const u32 symbol_offset = static_cast<u32>(msg.find(std::format("{}55", settings::SOH).c_str()));
            const symbol symbol_value = get_symbol(msg.substr(symbol_offset + 4, 20)); // xxxx...|270
            const range_t skip_size = SYMBOL_SKIP_SIZES[static_cast<u64>(symbol_value)];

            // Start inserting from index 1 as index 0 is reserved for error values
            u32 search_offset = 0;
            u32 insert_idx = 1;

            if constexpr (stride > 1) {
                // Keep looping untill message data cannot be aligned to stride anymore. 
                while (bitwise::gte(static_cast<i32>(msg.size() - search_offset), static_cast<i32>(stride))) {
                    /*std::cout << "msg = " << msg.substr(search_offset, 32) << std::endl;*/
                    
                    i32 new_entry_idx = find<stride, 2U>(
                        msg.data() + search_offset, std::format("{}279", settings::SOH).c_str()
                    );

                    i32 new_entry_found = bitwise::ne(new_entry_idx, -1);
                    u32 new_entry_found_mask = 0 - new_entry_found;
                    u32 msg_found_offset = (search_offset + new_entry_idx) & new_entry_found_mask;

                    entry_indices[insert_idx & new_entry_found_mask] = msg_found_offset;
                    insert_idx += new_entry_found;

                    i32 msg_action = msg[msg_found_offset + 5] - '0';
                    u32 is_remove_action_mask = 0 - bitwise::eq(msg_action, static_cast<i32>(md_action::remove));

                    search_offset += (
                        (4U & ~new_entry_found_mask) +
                        (new_entry_found_mask & (
                            ((skip_size.end + new_entry_idx) & ~is_remove_action_mask) +
                            ((skip_size.begin) & is_remove_action_mask)
                            ))
                        );

                }

                // Check if last part could contain another entry. 
                i32 new_entry_idx = static_cast<i32>(msg.find(std::format("{}279", settings::SOH).c_str(), search_offset));
                i32 new_entry_found = bitwise::ne(new_entry_idx, -1);
                u32 new_entry_found_mask = 0 - new_entry_found;
                u32 msg_found_offset = (search_offset + new_entry_idx) & new_entry_found_mask;
                entry_indices[insert_idx & new_entry_found_mask] = msg_found_offset;

                // Post fill last known entry index to be msg_size. 
                entry_indices[insert_idx + new_entry_found] = static_cast<u16>(msg.size());

            }
            else {

                search_offset = static_cast<u32>(msg.find(std::format("{}279", settings::SOH), 0));
                for (u32 i = 0; i < num_entries; i++) {
                    entry_indices[insert_idx++] = pos;
                    search_offset = static_cast<u32>(msg.find(std::format("{}279", settings::SOH), pos + 1));
                }

                // Post fill last known entry index to be msg_size. 
                entry_indices[num_entries + 1] = static_cast<u16>(msg.size());

            }

            // Filter found entry indices based on its size to determine remove of insert action. 
            // Increment count if either found and insert in seperate filter buffer. 
            const auto [insert_action_count, remove_action_count] = get_action_counts(num_entries, skip_size.end);
            
            // Generate and insert differnt kind of market_data based on remove or insert action.  
            u32 market_data_insert_idx = 0;
            for (u32 i = 2; i <= insert_action_count*2; i+=2) {
                new_insert_entry(
                    msg.substr(action_filter[i], action_filter[i+1U] - action_filter[i]),
                    market_data_insert_idx++, 
                    symbol_value
                );
            }

            for (u32 i = 2; i <= remove_action_count * 2; i += 2) {
                new_remove_entry(
                    msg.substr(
                        action_filter[remove_action_offset + i],
                        action_filter[remove_action_offset + i + 1U] - action_filter[remove_action_offset + i]
                    ), 
                    market_data_insert_idx++, 
                    symbol_value
                );
            }
            
            return num_entries;

        };

    private:
        u16 entry_indices[BufferSize] = { 0U };
        u16 action_filter[BufferSize * 4U] = { 0U };

        inline std::tuple<u32, u32> get_num_entries(std::string_view chunk) {
            const u32 entry_digit_size = static_cast<u32>(chunk.find(settings::SOH_CHAR));

            return { to_unsigned_integral<u32>(chunk.data(), entry_digit_size), entry_digit_size };
        }

        inline std::tuple<u32, u32> get_action_counts(const u32 num_entries, const u32 min_insert_action_size) {
            // Set correct array index offset for md_action::remove entries.
            static constexpr u32 remove_action_offset = static_cast<u32>(BufferSize);
            
            u32 remove_action_count = 1;
            u32 insert_action_count = 1;

            for (u32 i = 1; i <= num_entries + 1; i++) {
                u32 begin = entry_indices[i];
                u32 end = entry_indices[i + 1];
                u32 size = end - begin;

                i32 is_valid_offset = bitwise::ne(size, 0) & bitwise::lt(begin, end);
                i32 is_remove_action = bitwise::lte(size, min_insert_action_size) & is_valid_offset;
                u32 valid_offset_mask = 0 - is_valid_offset;
                u32 remove_action_mask = 0 - is_remove_action;

                const u32 action_idx_begin = (
                    ((remove_action_offset + remove_action_count) & remove_action_mask) +
                    ((insert_action_count) & ~remove_action_mask)
                ) & valid_offset_mask;

                const u32 action_idx_end = ((action_idx_begin + 1) & valid_offset_mask);

                action_filter[action_idx_begin+1] = begin;
                action_filter[action_idx_end+1] = end;

                // Insert entries every 2 indexes away to prevent overlapping sequences. 
                remove_action_count += ((is_remove_action) & is_valid_offset) * 2;
                insert_action_count += (((is_remove_action ^ 1)) & is_valid_offset) * 2;
            }

            return { (insert_action_count - 1) / 2, (remove_action_count - 1) / 2 };
        }

        inline symbol get_symbol(std::string_view chunk) {
            const u32 symbol_digit_size = find<20>(chunk.data(), settings::SOH_CHAR);
            return static_cast<symbol>(to_unsigned_integral<u64>(chunk.data(), symbol_digit_size));
        }

        inline void new_insert_entry(std::string_view chunk, const u32 entry_idx, const symbol sym) {
                // example: |279=0|269=1|278=0000002291667248|55=00000000000000000001|270=0000000001.08754|271=5000000
                auto& entry = market_data[entry_idx];

                entry.update_action = md_action::insert;
                entry.entry_type = static_cast<md_type>(chunk[11] - '0');
         
                u32 entry_idx_digit_count = find<16U>(
                    chunk.substr(17U, 16U).data(), settings::SOH_CHAR
                );

                entry.entry_id = to_unsigned_integral<u64>(chunk.substr(17U, entry_idx_digit_count));
                entry.symbol = sym;


                u32 entry_price_offset = 17U + entry_idx_digit_count + 4U + SYMBOL_DIGIT_SIZE[static_cast<u64>(sym)] + 5U;
                u32 entry_price_digit_count = find<16U>(
                    chunk.substr(entry_price_offset, 16U).data(),
                    settings::SOH_CHAR
                );

                u32 entry_size_offset = entry_price_offset + entry_price_digit_count + 5U;
                u32 entry_size_digit_count = static_cast<u32>(chunk.size() - entry_size_offset);
                
                entry.entry_size = to_unsigned_integral<u64>(
                    chunk.substr(entry_size_offset, entry_size_digit_count)
                );

                // insert entry_price
                // ...
        }

        inline void new_remove_entry(std::string_view chunk, const u32 entry_idx, const symbol sym) {
            // example: |279=2|278=2291666392|55=1
            auto& entry = market_data[entry_idx];

            entry.update_action = md_action::remove;

            u32 entry_idx_digit_count = static_cast<u32>(chunk.size() - (11U + 4U + SYMBOL_DIGIT_SIZE[static_cast<u64>(sym)]));

            entry.entry_id = to_unsigned_integral<u64>(chunk.substr(11U, 10U));
            entry.symbol = sym;
        }

    };

}
}