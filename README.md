# ctrader-fix-cpp

A Deterministic, branchless, and low latency FIX4.4 parser for the cTrader FIX API written in C++20 with AVX, AVX2, and AVX512 intrinsic support. Decode: 40ns/entry, 1us/msg. Encode: 80ns/msg.

The API is both scalable and easy to use, whilist still being performant. This is mainly due to the fact that any user configuration applied is done at compile time, unlike libraries like FIX8 or QuickFIX which require you to construct your own message buffer at run time. 

The API can be used as followed. 

## Encoding a message

```c++
#include <iostream>
#include <ctrader/encoder.hpp>

int main()
{

	ctrader::encode::encoder<connection::quote> encoder;
	encoder.encode<request::market_data_req>("ds32f3", subscription::enable, market_depth::full, symbol::eur_usd);
	auto em = encoder.get_buffer<request::market_data_req>();

	std::cout << em << std::endl;

	std::cout << std::endl;
	std::cout
		<< em.header << std::endl
		<< "\t" << em.header.entry.begin_string << std::endl
		<< "\t" << em.header.entry.body_length << std::endl
		<< "\t" << em.header.entry.msg_type << std::endl
		<< "\t" << em.header.entry.sender_comp_id << std::endl
		<< "\t" << em.header.entry.target_comp_id << std::endl
		<< "\t" << em.header.entry.target_sub_id << std::endl
		<< "\t" << em.header.entry.msg_seq_num << std::endl
		<< "\t" << em.header.entry.sending_time << std::endl;

	std::cout << std::endl;
	std::cout << em.body << std::endl;

	std::cout << std::endl;
	std::cout << em.trailer << std::endl;

	return 0;
}
```

### Custom modification can be added using a compile time struct like this:

```c++

...

constexpr ctrader::encode::encode_settings_t OverrideSettings = {
	.fix_version = "FIX.2.2",
	.user_name = "ArsalanAnwari",
	.password = "Aanwari#0609",

	.heartbeat_sec = "5",

	.sender_comp_id = "demo.somebroker.io",
	.target_comp_id = "cServer",
	.target_sub_id = { "QUOTE", "TRADE" },

	.date_time_mask = "00000000-00:00:00.000000",
	.date_time_mask_offsets = default_utc_time_offset,

	.symbol_is_digit_only = true,
	.max_symbol_digits = 5u,

	.max_msg_size_digits = 4u,
	.max_msg_type_digits = 1u,

	.max_seq_num_digits = 12u,
	.max_test_id_digits = 5u,
	.max_req_id_digits = 7u
};


int main() {

	ctrader::encode::encoder<connection::quote, OverrideSettings> encoder;
	...

}

```

# To do:

- [ ] Finish wiki:
- [ ] Replace permutations with `_mm256_alignr_epi8(v, zero_v, 8)` in `find()` function.
- [ ] Template unroll number conversion functions in `convert.hpp`
- [ ] bitwise comparisons for `price_t::operator<()`
- [ ] Use two `i32` members for `price_t`, one for value one for position of decimal. 
- [ ] Fix bug in `exec_policy` calculations when using stride with lower cascading technology. Ex: stride 20 with AVX2 support, should return `exec_policy::AVX` and not `exec_policy::AVX2` as (20 < 32) and (20 - ((20 % 16) + 16) == 0). 
- [ ] Change vectorized functions which work with none aligned data sizes to use padding data instead of a seperate scalar implementation for the leftover data. 