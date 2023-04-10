#pragma once

#include <algorithm>
#include <string>
#include <initializer_list>
#include <array>
#include <span>

#include "../types/packet.hpp"
#include "../types/encode.hpp"
#include "../types/packet/header.hpp"
#include "../tools/type_converter.hpp"
#include "../settings.hpp"

namespace {
	using namespace ctrader;

	using field_t = struct {
		std::string_view key;
		std::string_view value;
	};

	template<message T>
	consteval u16 calc_body_length(packet_t<T>& buff) {
		const auto header_part = sizeof(buff.header.raw) - 
			(sizeof(buff.header.entry.begin_string) + sizeof(buff.header.entry.body_length));

		return header_part + sizeof(buff.body.raw) + sizeof(buff.trailer.raw) - 2U;
	};

	template<message T>
	consteval packet_t<T> new_packet_from_fields(
		connection conn, std::initializer_list<field_t> body_fields
	) {
		packet_t<T> out;
		std::string buff = "8=FIX.4.4";
		char body_length[3] = {'0', '0', '0'};
	
		to_chars(std::span<char>(body_length), calc_body_length(out));

		// MSVC std::string implemntation is mentally retarted so therefore 
		// I need to manually construct a new string from an initializer list
		// instead of just passing a char* pointer... 
		const std::array<field_t, 7> header_fields = {
			field_t{"9", std::string({body_length[0], body_length[1], body_length[2]})},
			field_t{"35", MESSAGE_ID_VAL[static_cast<u8>(T)]},
			field_t{"34", std::string(settings::MAX_SEQ_NUM_DIGITS, '0')},
			field_t{"52", std::string(24, '0')},
			field_t{"49", settings::broker::SENDER_COMP_ID},
			field_t{"56", "CSERVER"},
			field_t{"57", CONNECTION_NAME[static_cast<u8>(conn)]}
		};

		for (const auto& field : header_fields) {
			buff += settings::SOH;
			buff += field.key;
			buff += "=";
			buff += field.value;
		}

		for (const auto& field : body_fields) {
			buff += settings::SOH;
			buff += field.key;
			buff += "=";
			buff += field.value;
		}

		buff += settings::SOH;
		buff += "10=000";
		buff += settings::SOH;

		std::copy(buff.begin(), buff.end(), out.data);
		return out;
	};

}

namespace ctrader {
namespace encode {

	template<message T>
	consteval auto new_packet(connection conn) {
		return new_packet_from_fields<T>(conn, {
			{"112", std::string("TEST")}
		});
	};

	template<> consteval auto 
	new_packet<message::logon>(connection conn) {
		return new_packet_from_fields<message::logon>(conn, {
			{"98", "0"},
			{"108", settings::HEARTBEAT_SEC},
			{"141", "Y"},
			{"553", settings::broker::USER_NAME},
			{"554", settings::broker::PASSWORD}
		});
	};

	template<> consteval auto
	new_packet<message::market_data_req>(connection conn) {
		return new_packet_from_fields<message::market_data_req>(conn, {
			{"262", std::string(settings::MAX_REQ_ID_DIGITS, '0')},
			{"263", "0"},
			{"264", "0"},
			{"265", "1"},
			{"267", "2"},
			{"269", "0"},
			{"269", "1"},
			{"146", "1"},
			{"55", std::string(19, '0') }
		});
	};

}
}