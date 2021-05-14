#pragma once

#include <algorithm>
#include <functional>
#include <string_view>
#include <vector>


namespace string {

	struct string_hash {
		using is_transparent = void;
		size_t operator()(const std::string_view value) const { return std::hash<std::string_view>{}(value); }
	};

	static constexpr std::string_view Trim(std::string_view string, const std::string_view delimiter = " \t\r\n") {
		string.remove_prefix(std::min<>(string.find_first_not_of(delimiter), string.size()));
		string.remove_suffix(string.size() - string.find_last_not_of(delimiter) - 1);
		return string;
	}

	static std::vector<std::string_view> Split(const std::string_view string, const std::string_view delimiter) {
		std::vector<std::string_view> tokens;
		for (auto i = string.find_first_not_of(delimiter); i < string.size();) {
			const auto j = std::min<>(string.find_first_of(delimiter, i), string.size());
			tokens.push_back(string.substr(i, j - i));
			i = string.find_first_not_of(delimiter, j);
		}
		return tokens;
	}
}
