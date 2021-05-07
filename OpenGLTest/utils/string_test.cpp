#include <gtest/gtest.h>

#include "utils/string.hpp"

namespace {

	TEST(String, TestEmptyStringStartsWithPrefix) {
		constexpr auto* line = "";
		constexpr auto* prefix = "Hello";
		static_assert(!string::StartsWith(line, prefix));
	}

	TEST(String, TestStringStartsWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "Hello";
		static_assert(string::StartsWith(line, prefix));
	}

	TEST(String, TestStringDoesNotStartWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "World!";
		static_assert(!string::StartsWith(line, prefix));
	}

	TEST(String, TestShortStringDoesNotStartWithLongPrefix) {
		constexpr auto* line = "Hello";
		constexpr auto* prefix = "Hello, World!";
		static_assert(!string::StartsWith(line, prefix));
	}

	TEST(String, TestTrimWhitespaceString) {
		constexpr auto* line = "     ";
		static_assert(string::Trim(line).empty());
	}

	TEST(String, TestTrimString) {
		constexpr auto line = "\t  Hello, World!  \t\r\n";
		static_assert("Hello, World!" == string::Trim(line));
	}

	TEST(String, TestSplitEmptyString) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("", delimiter);
		ASSERT_TRUE(tokens.empty());
	}

	TEST(String, TestSplitWhitespaceString) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("  ", delimiter);
		ASSERT_TRUE(tokens.empty());
	}

	TEST(String, TestSplitNoWhitespaceString) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("Hello", delimiter);
		ASSERT_EQ((std::vector<std::string_view>{"Hello"}), tokens);
	}

	TEST(String, TestSplitStringOnWhitespaceAndTab) {
		constexpr auto* delimiter = " \t";
		const auto tokens = string::Split("\t vt 0.707 0.395 0.684 ", delimiter);
		ASSERT_EQ((std::vector<std::string_view>{"vt", "0.707", "0.395", "0.684"}), tokens);
	}
}