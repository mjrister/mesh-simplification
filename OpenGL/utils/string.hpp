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

	static std::vector<std::string_view> Split(const std::string_view string, const std::string_view delimiter) {

		if (string.empty()) return {};

		auto i = string.find_first_of(delimiter);
		if (i == std::string_view::npos) return { string };

		i = string.find_first_not_of(delimiter);
		if (i == std::string_view::npos) return {};

		std::vector<std::string_view> tokens;
		while(i != std::string_view::npos) {
			const auto j = std::min<std::size_t>(string.find_first_of(delimiter, i), string.size());
			tokens.push_back(string.substr(i, j - i));
			i = string.find_first_not_of(delimiter, j);
		}

		return tokens;
	}
}
