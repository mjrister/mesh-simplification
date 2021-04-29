#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

namespace utils {

	static constexpr bool StartsWith(const std::string_view line, const std::string_view prefix) {
		return line.compare(0, prefix.size(), prefix.data()) == 0;
	}

	static std::vector<std::string_view> Split(const std::string_view line, const std::string_view delimiter) {
		std::vector<std::string_view> tokens;
		for (auto i = line.find_first_not_of(delimiter); i != std::string_view::npos;) {
			const auto j = std::min(line.find_first_of(delimiter, i), line.size());
			tokens.push_back(line.substr(i, j - i));
			i = line.find_first_not_of(delimiter, j);
		}
		return tokens;
	}
}
