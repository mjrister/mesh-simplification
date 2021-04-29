#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

namespace utils {

	static constexpr std::string_view Trim(std::string_view line) noexcept {
		line.remove_prefix(std::min(line.find_first_not_of(' '), line.size()));
		line.remove_suffix(line.size() - line.find_last_not_of(' ') - 1);
		return line;
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
