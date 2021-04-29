#include <gtest/gtest.h>

#include "utils/string.hpp"

namespace {

	TEST(String, TestEmptyStringStartsWithPrefix) {
		constexpr auto* line = "";
		constexpr auto* prefix = "Hello";
		ASSERT_FALSE(string::StartsWith(line, prefix));
	}

	TEST(String, TestStringStartsWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "Hello";
		ASSERT_TRUE(string::StartsWith(line, prefix));
	}

	TEST(String, TestStringDoesNotStartWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "World!";
		ASSERT_FALSE(string::StartsWith(line, prefix));
	}

	TEST(String, TestShortStringDoesNotStartWithLongPrefix) {
		constexpr auto* line = "Hello";
		constexpr auto* prefix = "Hello, World!";
		ASSERT_FALSE(string::StartsWith(line, prefix));
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
		const auto* delimiter = " ";
		const auto tokens = string::Split("Hello", delimiter);
		ASSERT_EQ(tokens.size(), 1);
		ASSERT_EQ("Hello", tokens[0]);
	}

	TEST(String, TestSplitStringOnWhitespace) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("   vt 0.707  0.395 0.684   ", delimiter);
		ASSERT_EQ(tokens.size(), 4);
		ASSERT_EQ("vt", tokens[0]);
		ASSERT_EQ("0.707", tokens[1]);
		ASSERT_EQ("0.395", tokens[2]);
		ASSERT_EQ("0.684", tokens[3]);
	}
}