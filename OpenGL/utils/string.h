#pragma once

#include <algorithm>
#include <string_view>
#include <vector>


namespace string {

	/**
	 * \brief Determines if a string starts with a given prefix.
	 * \param string The string to evaluate.
	 * \param prefix The prefix to test.
	 * \return \c true if \p string starts with \p prefix, otherwise \c false.
	 * \note This can be removed in favor of \c std::string_view::starts_with when this project is migrated to C++20.
	 */
	static constexpr bool StartsWith(const std::string_view string, const std::string_view prefix) {
		return prefix.size() <= string.size() && string.compare(0, prefix.size(), prefix.data()) == 0;
	}

	/**
	 * \brief Removes a set of characters from the beginning and end of the string.
	 * \param string The string to evaluate.
	 * \param delimiter A set of characters (in any order) to remove from the beginning and end of the string.
	 * \return A view of the string with the characters if \p delimiter removed from the beginning and end of \p string.
	 * \warning Generally, returning a \c string_view is unsafe since it may lead to a dangling reference if a temporary
	 *          is passed in to \p string. This is guaranteed to \b not happen in this application since its usage is
	 *          primarily in the context of .obj parsing which always contains a reference to a line in the .obj file.
	 */
	static constexpr std::string_view Trim(std::string_view string, const std::string_view delimiter = " \t\r\n") {
		string.remove_prefix(std::min<>(string.find_first_not_of(delimiter), string.size()));
		string.remove_suffix(string.size() - string.find_last_not_of(delimiter) - 1);
		return string;
	}

	/**
	 * \brief Gets tokens delimited by a set of characters.
	 * \param string The string to evaluate.
	 * \param delimiter The set of characters (in any order) split the string on.
	 * \return A vector of tokens in \p string split on the characters in \p delimiter.
	 * \warning Generally, returning a \c string_view is unsafe since it may lead to a dangling reference if a temporary
	 *          is passed in to \p string. This is guaranteed to \b not happen in this application since its usage is
	 *          primarily in the context of .obj parsing which always contains a reference to a line in the .obj file.
	 */
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
