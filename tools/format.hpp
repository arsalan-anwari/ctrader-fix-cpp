#pragma once

#include <string>
#include <initializer_list>
#include <utility>

#include "../types/numbers.hpp"

namespace ctrader {

namespace json_format {

	constexpr std::string new_subgroup(const std::string& name, const std::string& sub_group) {
		std::string out;
		out += "\"" + name + "\"" + ":{" + sub_group + "}";
		return out;
	};

	constexpr std::string new_tag(const std::string& name, const std::string& value) {
		std::string out;
		out += "\"" + name + "\"" + ":" + "\"" + value + "\"";
		return out;
	};

	constexpr std::string new_list(const std::initializer_list<std::pair<std::string, std::string>>& entries) {
		std::string out;
		for (const auto& entry : entries) {
			out += new_tag(entry.first, entry.second) + ",";
		}
		out.pop_back();
		return out;
	};

}

}