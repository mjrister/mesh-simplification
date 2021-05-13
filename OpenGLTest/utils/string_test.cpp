#include <gtest/gtest.h>

#include "utils/string.hpp"

namespace {
	using namespace string;

	TEST(String, TestTrimWhitespaceString) {
		constexpr auto* line = "     ";
		static_assert(Trim(line).empty());
	}

	TEST(String, TestTrimString) {
		constexpr auto* line = "\t  Hello, World!  \t\r\n";
		static_assert("Hello, World!" == Trim(line));
	}

	TEST(String, TestSplitEmptyString) {
		constexpr auto* line = "";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(String, TestSplitWhitespaceString) {
		constexpr auto* line = "   ";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(String, TestSplitNoWhitespaceString) {
		constexpr auto* line = "Hello";
		const auto tokens = Split(line, " ");
		ASSERT_EQ((std::vector<std::string_view>{line}), tokens);
	}

	TEST(String, TestSplitStringOnWhitespaceAndTab) {
		constexpr auto* line = "\t vt 0.707 0.395 0.684 ";
		const auto tokens = Split(line, " \t");
		ASSERT_EQ((std::vector<std::string_view>{"vt", "0.707", "0.395", "0.684"}), tokens);
	}
}