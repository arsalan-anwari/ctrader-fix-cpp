#pragma once

#include <string>
#include <initializer_list>
#include <utility>

#include "../types/numbers.hpp"

namespace ctrader {
namespace json_format {

	constexpr auto new_list(const std::initializer_list<std::pair<std::string, std::string>>& entries) {
		std::string out;
		for (const auto& entry : entries) {
			out += std::string("{\"") + entry.first + "\":" + "\"" + entry.second + "\"},";
		}
		out.pop_back();
		return out;
	};

	constexpr auto new_group_list(const std::string& name, const std::string& list) -> std::string {
		std::string out;
		out += "\"" + name + "\"" + ":[" + list + "]";
		return out;

		//return std::format("\"{}\":[{}]", name, list);
	};

	constexpr auto new_subgroup(const std::string& name, const std::string& sub_group) -> std::string {
		std::string out;
		out += "\"" + name + "\"" + ":{" + sub_group + "}";
		return out;
		
		//return std::format("\"{}\":{}{}{}", name, "{", sub_group, "}");
	};

	constexpr auto new_tag(const std::string& name, const std::string& value) -> std::string {
		//return std::format("\"{}\":\"{}\"", name, value);
		std::string out;
		out += "\"" + name + "\"" + ":" + "\"" + value + "\"";
		return out;
	};

}
}