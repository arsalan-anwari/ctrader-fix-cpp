#pragma once

#include <algorithm>
#include <string>
#include <initializer_list>
#include <array>
#include <span>
#include <string_view>

#include "../encode_settings.hpp"

#include "../types/numbers.hpp"
#include "../types/packet.hpp"
#include "../types/encode.hpp"
#include "../debug_settings.hpp"

#include "../tools/convert.hpp"

namespace {
	using namespace ctrader;

	using field_t = struct {
		std::string_view key;
		std::string_view value;
	};

	template<encode_options Settings, request T>
	constexpr u16 calc_body_length(packet_t<Settings, T>& buff) {
		const auto header_part = sizeof(buff.header.raw)
			- sizeof(buff.header.entry.begin_string.raw)
			- sizeof(buff.header.entry.body_length.raw);

		return header_part + sizeof(buff.body.raw) + 1U;
	};

	template<encode_options Settings, request T>
	constexpr packet_t<Settings, T> new_packet_from_fields(
		connection conn, std::initializer_list<field_t> body_fields
	) {
		packet_t<Settings, T> out;
		std::string buff = "8=";
		buff += std::string(Settings.fix_version);

		// Message sizes differ per packet type, so calculate the size in advance. 
		char body_length[Settings.max_msg_size_digits + 1] = { '0' };
		from_intergral(std::span(body_length, Settings.max_msg_size_digits), calc_body_length(out));

		buff += debug_settings::SOH;
		buff += "9=";
		buff += std::string(body_length);

		const std::array<field_t, 6> header_fields = {
			field_t{ "35", REQUEST_ID_VAL[static_cast<u8>(T) - 1] },
			field_t{ "49", std::string(Settings.sender_comp_id) },
			field_t{ "56", std::string(Settings.target_comp_id) },
			field_t{ "57", std::string(Settings.target_sub_id[static_cast<u8>(conn)]) },
			field_t{ "34", std::string(Settings.max_seq_num_digits, '0') },
			field_t{ "52", Settings.date_time_mask }
		};

		for (const auto& field : header_fields) {
			buff += debug_settings::SOH;
			buff += field.key;
			buff += "=";
			buff += field.value;
		}

		for (const auto& field : body_fields) {
			buff += debug_settings::SOH;
			buff += field.key;
			buff += "=";
			buff += field.value;
		}

		buff += debug_settings::SOH;
		buff += "10=000";
		buff += debug_settings::SOH;

		std::copy(buff.begin(), buff.end(), out.data);
		return out;
	};

}

namespace ctrader {
namespace encode {

	template<connection ConnectionType, encode_options Settings, request Request>
	struct packet_generator {
		static constexpr auto data = new_packet_from_fields<Settings, Request>(ConnectionType, {
			{"112", std::string("TEST")}
		});
	};

	template<connection ConnectionType, encode_options Settings>
	struct packet_generator<ConnectionType, Settings, request::logon> {
		static constexpr auto heartbeat_sec = ctrader::as_cv_string<Settings.heartbeat_sec>();

		static constexpr auto data = new_packet_from_fields<Settings, request::logon>(ConnectionType, {
			{"98", "0"},
			{"108", std::string(heartbeat_sec.data, heartbeat_sec.length)},
			{"141", "Y"},
			{"553", std::string(Settings.user_name)},
			{"554", std::string(Settings.password)}
		});
	};

	template<connection ConnectionType, encode_options Settings>
	struct packet_generator<ConnectionType, Settings, request::market_data_req> {
		static constexpr auto data = new_packet_from_fields<Settings, request::market_data_req>(ConnectionType, {
			{"262", std::string(Settings.max_req_id_digits, '0')},
			{"263", "0"},
			{"264", "0"},
			{"265", "1"},
			{"267", "2"},
			{"269", "0"},
			{"269", "1"},
			{"146", "1"},
			{"55", std::string(Settings.max_symbol_digits, '0') }
		});
	};


	/*constexpr auto new_packet(connection conn) {
		return new_packet_from_fields<T>(conn, Settings, {
			{"112", std::string("TEST")}
		});
	};*/

	/*template<encode_options Settings>
	constexpr auto new_packet<Settings, request::logon>(connection conn) {
		return new_packet_from_fields<Settings, request::logon>(conn, {
			{"98", "0"},
			{"108", std::to_string(Settings.heartbeat_sec)},
			{"141", "Y"},
			{"553", std::string(Settings.user_name)},
			{"554", std::string(Settings.password)}
		});
	};*/

	//template<encode_options Settings>
	//constexpr auto new_packet<Settings, request::market_data_req>(connection conn) {
	//	return new_packet_from_fields<Settings, request::market_data_req>(conn, {
	//		{"262", std::string(Settings.max_req_id_digits, '0')},
	//		{"263", "0"},
	//		{"264", "0"},
	//		{"265", "1"},
	//		{"267", "2"},
	//		{"269", "0"},
	//		{"269", "1"},
	//		{"146", "1"},
	//		{"55", std::string(Settings.max_symbol_digits, '0') }
	//	});
	//};


}} // namespace ctrader::encode 