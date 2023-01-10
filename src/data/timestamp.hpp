#pragma once

#include <algorithm>

#include "settings.hpp"
#include "tools/memory.hpp"
#include "tools/datetime.hpp"

namespace ctrader::data::timestamp {

    namespace internal {

        using namespace ctrader::settings;
        using namespace ctrader::tools;

        consteval bool is_valid_format(){
            static_assert( 
                broker_settings::SendingTimeFormat.size() < 28,  
                "Format 'SendingTimeFormatCustom' in file 'settings.hpp' not supported as it is too big to fit within 32 characters!"
            );
            
            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%YYY", 0, 4),
                "Could not find option '%YYY' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%m", 0, 2),
                "Could not find option '%m' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%d", 0, 2),
                "Could not find option '%d' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%H", 0, 2),
                "Could not find option '%H' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%M", 0, 2),
                "Could not find option '%M' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%S", 0, 2),
                "Could not find option '%S' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            static_assert(
                broker_settings::SendingTimeFormat.npos != broker_settings::SendingTimeFormat.find("%fffff", 0, 6),
                "Could not find option '%fffff' in variable 'SendingTimeFormatCustom' in file 'settings.hpp' !"
            );

            return true;
        }

        consteval uint8_t offset_from_key(const char* key, uint8_t key_size){
            return static_cast<uint8_t>(broker_settings::SendingTimeFormat.find(key, 0, key_size));
        }

        consteval datetime::gmt_offset_t gmt_offset_from_format(){
            static_assert(
                is_valid_format() == true,
                "Format 'SendingTimeFormatCustom' in file 'settings.hpp' is invalid!"
            );

            // offset +1 because of '|52=' characters in beginning
            return datetime::gmt_offset_t {
                .year_offset = offset_from_key("%YYY", 4) + 4,
                .month_offset = offset_from_key("%m", 2) + 4,
                .day_offset = offset_from_key("%d", 2) + 4,
                .hour_offset = offset_from_key("%H", 2) + 4,
                .min_offset = offset_from_key("%M", 2) + 4,
                .sec_offset = offset_from_key("%S", 2) + 4,
                .subsec_offset = offset_from_key("%fffff", 6) + 4
            };
        }

        template<std::size_t N>
        consteval memory::simple_buffer_t<char, N> timestamp_buffer_from_offset( const datetime::gmt_offset_t& offset ){
            memory::simple_buffer_t<char, N> buff;
            char* fmt = buff.data;

            std::copy(
                broker_settings::SendingTimeFormat.begin(),
                broker_settings::SendingTimeFormat.end(),
                buff.data
            );

            std::fill_n( fmt+offset.year_offset-4, 4, '0' );
            std::fill_n( fmt+offset.month_offset-4, 2, '0' );
            std::fill_n( fmt+offset.day_offset-4, 2, '0' );

            std::fill_n( fmt+offset.hour_offset-4, 2, '0' );
            std::fill_n( fmt+offset.min_offset-4, 2, '0' );
            std::fill_n( fmt+offset.sec_offset-4, 2, '0' );
            std::fill_n( fmt+offset.subsec_offset-4, 6, '0' );

            return buff;
        }

    } // namespace internal

    constexpr auto TimestampSize = ctrader::settings::broker_settings::SendingTimeFormat.size();
    constexpr auto TimestampOffset = internal::gmt_offset_from_format();
    constexpr auto TimestampBuffer = internal::timestamp_buffer_from_offset<TimestampSize+1>( TimestampOffset );

    
} // namespace ctrader::data::timestamp
