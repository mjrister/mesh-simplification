#include <gtest/gtest.h>

#include "utils/string.h"

namespace {
	using namespace string;

	TEST(StringTest, TestEmptyStringStartsWithPrefix) {
		constexpr auto* line = "";
		static_assert(!StartsWith(line, "Hello"));
	}

	TEST(StringTest, TestStringStartsWithPrefix) {
		constexpr auto* line = "Hello, World!";
		static_assert(StartsWith(line, "Hello"));
	}

	TEST(StringTest, TestStringDoesNotStartWithPrefix) {
		constexpr auto* line = "Hello, World!";
		static_assert(!StartsWith(line, "World!"));
	}

	TEST(StringTest, TestShortStringDoesNotStartWithLongPrefix) {
		constexpr auto* line = "Hello";
		static_assert(!StartsWith(line, "Hello, World!"));
	}

	TEST(StringTest, TestTrimWhitespaceString) {
		constexpr auto* line = "     ";
		static_assert(Trim(line).empty());
	}

	TEST(StringTest, TestTrimString) {
		constexpr auto* line = "\t  Hello, World!  \t\r\n";
		static_assert("Hello, World!" == Trim(line));
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
		ASSERT_EQ((std::vector<std::string_view>{line}), tokens);
	}

	TEST(StringTest, TestSplitStringOnWhitespaceAndTab) {
		constexpr auto* line = "\t vt 0.707 0.395 0.684 ";
		const auto tokens = Split(line, " \t");
		ASSERT_EQ((std::vector<std::string_view>{"vt", "0.707", "0.395", "0.684"}), tokens);
	}
}