# ctrader-fix-cpp

A Deterministic, branchless, and low latency FIX4.4 parser for the cTrader FIX API written in C++20 with AVX, AVX2, and AVX512 intrinsic support. Decode: 40ns/entry, 1us/msg. Encode: 80ns/msg.

The API is both scalable and easy to use, whilist still being performant. This is mainly due to the fact that any user configuration applied is done at compile time, unlike libraries like FIX8 or QuickFIX which require you to construct your own message buffer at run time. 

The API can be used as followed. 

## Encoding a message

```c++
int main()
{

	ctrader::encode::encoder<connection::quote> encoder;

	/// Logon

	encoder.encode<request::logon>();
	auto logon = encoder.get_buffer<request::logon>();

	std::cout << logon << std::endl;

	std::cout << std::endl;
	std::cout
		<< logon.header << std::endl
		<< "\t" << logon.header.entry.begin_string << std::endl
		<< "\t" << logon.header.entry.body_length << std::endl
		<< "\t" << logon.header.entry.msg_type << std::endl
		<< "\t" << logon.header.entry.sender_comp_id << std::endl
		<< "\t" << logon.header.entry.target_comp_id << std::endl
		<< "\t" << logon.header.entry.target_sub_id << std::endl
		<< "\t" << logon.header.entry.msg_seq_num << std::endl
		<< "\t" << logon.header.entry.sending_time << std::endl;

	std::cout << std::endl;
	std::cout
		<< logon.body << std::endl
		<< "\t" << logon.body.entry.encrypt_method << std::endl
		<< "\t" << logon.body.entry.heartbeat << std::endl
		<< "\t" << logon.body.entry.reset_seq_num_flag << std::endl
		<< "\t" << logon.body.entry.username << std::endl
		<< "\t" << logon.body.entry.password << std::endl;

	std::cout << std::endl;
	std::cout << logon.trailer << std::endl;
	std::cout << "\t -> msg_seq_num = " << encoder.get_seq_num() << std::endl << std::endl;


	/// Market Data Request

	encoder.encode<request::market_data_req>("ds32f3", subscription::enable, market_depth::full, symbol::eur_usd);
	auto market_data_req = encoder.get_buffer<request::market_data_req>();

	std::cout << market_data_req << std::endl;

	std::cout << std::endl;
	std::cout
		<< market_data_req.header << std::endl
		<< "\t" << market_data_req.header.entry.begin_string << std::endl
		<< "\t" << market_data_req.header.entry.body_length << std::endl
		<< "\t" << market_data_req.header.entry.msg_type << std::endl
		<< "\t" << market_data_req.header.entry.sender_comp_id << std::endl
		<< "\t" << market_data_req.header.entry.target_comp_id << std::endl
		<< "\t" << market_data_req.header.entry.target_sub_id << std::endl
		<< "\t" << market_data_req.header.entry.msg_seq_num << std::endl
		<< "\t" << market_data_req.header.entry.sending_time << std::endl;

	std::cout << std::endl;
	std::cout 
		<< market_data_req.body << std::endl
		<< "\t" << market_data_req.body.entry.md_req_id << std::endl
		<< "\t" << market_data_req.body.entry.subscription_req_type << std::endl
		<< "\t" << market_data_req.body.entry.market_depth << std::endl
		<< "\t" << market_data_req.body.entry.md_update_type << std::endl
		<< "\t" << market_data_req.body.entry.entry_type_count << std::endl
		<< "\t" << market_data_req.body.entry.entry_bid << std::endl
		<< "\t" << market_data_req.body.entry.entry_offer << std::endl
		<< "\t" << market_data_req.body.entry.symbol_count << std::endl
		<< "\t" << market_data_req.body.entry.symbol << std::endl;

	std::cout << std::endl;
	std::cout << market_data_req.trailer << std::endl;
	std::cout << "\t -> msg_seq_num = " << encoder.get_seq_num() << std::endl << std::endl;

	return 0;
}
```

### Custom modification can be added using a compile time struct like this:

```c++

...

constexpr ctrader::encode_options OverrideSettings = {
	.fix_version = "FIX.2.2",
	.user_name = "ArsalanAnwari",
	.password = "Aanwari#0609",

	.sender_comp_id = "demo.blocktech.io",
	.target_comp_id = "cServer",
	.target_sub_id = { "QUOTE", "TRADE" },

	.date_time_mask = "00000000-00:00:00.000000",
	.date_time_mask_offsets = ctrader::default_utc_time_offset,

	.heartbeat_sec = 10u,

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

- [x] Rename types to use `type_name` syntax instead of `type_name_t`.
- [x] Seperate settings into `debug_settings.hpp`, `encode_settings.hpp` and `decode_settings.hpp` instead of using `settings.hpp`.
- [ ] Replace permutations with `_mm256_alignr_epi8(v, zero_v, 8)` in `find()` function.
- [ ] Template unroll `as_integral()` in `convert.hpp` for values with less than usize valued digits.
- [ ] Use bitwise comparisons for `price::operator<()` instead of logical operators
- [ ] Use two `i32` members for `price` instead of 3, one for value one for position of decimal. 
- [ ] Fix bug in `exec_policy` calculations when using stride with lower cascading technology. Ex: stride 20 with AVX2 support, should return `exec_policy::AVX` and not `exec_policy::AVX2` as (20 < 32) and (20 - ((20 % 16) + 16) == 0). 
- [ ] Change vectorized functions which work with none aligned data sizes to use padding data instead of a seperate scalar implementation for the leftover data. 
- [ ] Finish README with example and add wiki.
