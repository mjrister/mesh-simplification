#include <gtest/gtest.h>

#include "utils/string.h"

using namespace string;

namespace {
	TEST(StringTest, TestTrimWhitespaceString) {
		constexpr auto* line = "     ";
		ASSERT_TRUE(Trim(line).empty());
	}

	TEST(StringTest, TestTrimString) {
		const auto* line = "\t  Hello, World!  \t\r\n";
		ASSERT_EQ("Hello, World!", Trim(line));
	}

	TEST(StringTest, TestSplitEmptyString) {
		constexpr auto* line = "";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringTest, TestSplitWhitespaceString) {
		constexpr auto* line = "   ";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringTest, TestSplitNoWhitespaceString) {
		constexpr auto* line = "Hello";
		const auto tokens = Split(line, " ");
		ASSERT_EQ((std::vector<std::string>{line}), tokens);
	}

	TEST(StringTest, TestSplitStringOnWhitespaceAndTab) {
		constexpr auto* line = "\t vt 0.707 0.395 0.684 ";
		const auto tokens = Split(line, " \t");
		ASSERT_EQ((std::vector<std::string>{"vt", "0.707", "0.395", "0.684"}), tokens);
	}
}