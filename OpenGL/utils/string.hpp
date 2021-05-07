#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

namespace string {

	static constexpr bool StartsWith(const std::string_view string, const std::string_view prefix) {
		return prefix.size() <= string.size() && string.compare(0, prefix.size(), prefix.data()) == 0;
	}

	static constexpr std::string_view Trim(std::string_view string, const std::string_view delimiter = " \t\r\n") {
		string.remove_prefix(std::min(string.find_first_not_of(delimiter), string.size()));
		string.remove_suffix(string.size() - string.find_last_not_of(delimiter) - 1);
		return string;
	}

	static std::vector<std::string_view> Split(std::string_view string, const std::string_view delimiter) {

		string = Trim(string, delimiter);
		if (string.empty()) return {};

		std::vector<std::string_view> tokens;
		for (std::size_t i = 0; i < string.size();) {
			const auto j = std::min<std::size_t>(string.find_first_of(delimiter, i), string.size());
			tokens.push_back(string.substr(i, j - i));
			i = string.find_first_not_of(delimiter, j);
		}

		return tokens;
	}
}
