#pragma once

#include <algorithm>
#include <string>
#include <initializer_list>
#include <array>
#include <span>
#include <string_view>

#include "../types/numbers.hpp"
#include "../types/packet.hpp"
#include "../types/encode.hpp"
#include "../settings.hpp"

#include "../tools/convert.hpp"

namespace {
	using namespace ctrader;

	using field_t = struct {
		std::string_view key;
		std::string_view value;
	};

	template<request T>
	constexpr u16 calc_body_length(packet_t<T>& buff) {
		const auto header_part = sizeof(buff.header.raw)
			- sizeof(buff.header.entry.begin_string.raw)
			- sizeof(buff.header.entry.body_length.raw);

		return header_part + sizeof(buff.body.raw) + 1U;
	};

	template<request T>
	constexpr packet_t<T> new_packet_from_fields(
		connection conn, std::initializer_list<field_t> body_fields
	) {
		packet_t<T> out;
		std::string buff = "8=";
		buff += settings::broker::FIX_VERSION;

		// Message sizes differ per packet type, so calculate the size in advance. 
		std::string body_length = { '0', '0', '0' };
		from_intergral(std::span<char>(body_length.data(), body_length.size()), calc_body_length(out));

		buff += settings::SOH;
		buff += "9=";
		buff += body_length;

		const std::array<field_t, 6> header_fields = {
			field_t{"35", REQUEST_ID_VAL[static_cast<u8>(T) - 1]},
			field_t{"49", settings::broker::SENDER_COMP_ID},
			field_t{"56", settings::TARGET_COMP_ID},
			field_t{"57", settings::TARGET_SUB_ID[static_cast<u8>(conn)]},
			field_t{"34", std::string(settings::MAX_SEQ_NUM_DIGITS, '0')},
			field_t{"52", settings::DATE_TIME_MASK}
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

	template<request T>
	constexpr auto new_packet(connection conn) {
		return new_packet_from_fields<T>(conn, {
			{"112", std::string("TEST")}
		});
	};

	template<> 
	constexpr auto new_packet<request::logon>(connection conn) {
		return new_packet_from_fields<request::logon>(conn, {
			{"98", "0"},
			{"108", std::to_string(settings::HEARTBEAT_SEC)},
			{"141", "Y"},
			{"553", settings::broker::USER_NAME},
			{"554", settings::broker::PASSWORD}
		});
	};

	template<> 
	constexpr auto new_packet<request::market_data_req>(connection conn) {
		return new_packet_from_fields<request::market_data_req>(conn, {
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


}} // namespace ctrader::encode 