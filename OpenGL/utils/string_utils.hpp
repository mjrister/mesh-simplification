#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

namespace string {

	static constexpr bool StartsWith(const std::string_view string, const std::string_view prefix) {
		return prefix.size() <= string.size() && string.compare(0, prefix.size(), prefix.data()) == 0;
	}

	static std::vector<std::string_view> Split(const std::string_view string, const std::string_view delimiter) {
		std::vector<std::string_view> tokens;
		for (auto i = string.find_first_not_of(delimiter); i != std::string_view::npos;) {
			const auto j = std::min<std::size_t>(string.find_first_of(delimiter, i), string.size());
			tokens.push_back(string.substr(i, j - i));
			i = string.find_first_not_of(delimiter, j);
		}
		return tokens;
	}
}
