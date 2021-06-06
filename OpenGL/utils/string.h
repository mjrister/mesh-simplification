#pragma once

#include <algorithm>
#include <string_view>
#include <vector>


namespace string {

	/**
	 * \brief Implements the hash operator for a string view. Needed to perform heterogeneous lookup in unordered containers.
	 */


	/**
	 * \brief Removes a set of characters from the beginning and end of the string.
	 * \param line The string to evaluate.
	 * \param delimiter A set of characters (in any order) to remove from the beginning and end of the string.
	 * \return A string with the characters in \p delimiter removed from the beginning and end of \p line.
	 */
	inline std::string Trim(std::string_view line, const std::string_view delimiter = " \t\r\n") {
		line.remove_prefix(std::min<>(line.find_first_not_of(delimiter), line.size()));
		line.remove_suffix(line.size() - line.find_last_not_of(delimiter) - 1);
		return std::string{line};
	}

	/**
	 * \brief Gets tokens delimited by a set of characters.
	 * \param line The string to evaluate.
	 * \param delimiter The set of characters (in any order) to split the string on.
	 * \return A vector of tokens in \p line split on the characters in \p delimiter.
	 */
	inline std::vector<std::string> Split(const std::string_view line, const std::string_view delimiter) {
		std::vector<std::string> tokens;
		for (auto i = line.find_first_not_of(delimiter); i < line.size();) {
			const auto j = std::min<>(line.find_first_of(delimiter, i), line.size());
			tokens.emplace_back(line.substr(i, j - i));
			i = line.find_first_not_of(delimiter, j);
		}
		return tokens;
	}
}
