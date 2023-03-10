#pragma once

#include "decode_method.hpp"
#include "types/concepts.hpp"

namespace ctrader::parser::decode {

    using namespace ctrader::parser::decode::methods;
    using namespace ctrader::types::concepts;

    template <u16 MAX_ENTRIES> 
    requires ( is_min_size<MAX_ENTRIES, 32> && is_max_size<MAX_ENTRIES, 65536> && is_power_of_2<MAX_ENTRIES> )
    struct Decoder {

        template <DECODE_TYPE D = DECODE_TYPE::UNKNOWN> u16 decode(const char* data){
            using namespace ctrader::tools;

            if constexpr( D == DECODE_TYPE::MARKET_DATA_INCREMENTAL ){ 
                const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH);
                return market_data_decoder.prepare_and_decode_incremental(data, msg_size_digit_size);  
            } else if ( D == DECODE_TYPE::UNKNOWN ) {
                const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH);
                const char msg_type = data[12 + msg_size_digit_size + 4];

                switch(msg_type){
                    case 'X': { return market_data_decoder.prepare_and_decode_incremental(data, msg_size_digit_size); }
                };

                return 0;
            } else {
                const u32 msg_size_digit_size = memory::find<5>(data+12, __SETTINGS_SOH);
                const char msg_type = data[12 + msg_size_digit_size + 4];

                switch(msg_type){
                    case 'X': { return market_data_decoder.prepare_and_decode_incremental(data, msg_size_digit_size); }
                };

                return 0;
            }
           
        };
        
        template <DATA_TYPE T> DATA_BUFF<T, MAX_ENTRIES> const& get_decode_data() const{
            if constexpr( T == DATA_TYPE::MARKET_DATA ){ return market_data_decoder.market_data; }
        };

    private:
        data_decoder<MAX_ENTRIES, DATA_TYPE::MARKET_DATA> market_data_decoder;

     
        // template<DECODE_TYPE T> __attribute__((optimize("unroll-loops")))
        // void decode_algorithm(const char* data, const u32 data_size, const u16 num_entries);

       

    };



} // ctrader::parser
