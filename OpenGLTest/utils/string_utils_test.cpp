#include <gtest/gtest.h>

#include "utils/string_utils.hpp"

namespace {

	TEST(StringUtils, TestEmptyStringStartsWithPrefix) {
		constexpr auto* line = "";
		constexpr auto* prefix = "Hello";
		ASSERT_FALSE(string::StartsWith(line, prefix));
	}

	TEST(StringUtils, TestStringStartsWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "Hello";
		ASSERT_TRUE(string::StartsWith(line, prefix));
	}

	TEST(StringUtils, TestStringDoesNotStartWithPrefix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "World!";
		ASSERT_FALSE(string::StartsWith(line, prefix));
	}

	TEST(StringUtils, TestShortStringDoesNotStartWithLongPrefix) {
		constexpr auto* line = "Hello";
		constexpr auto* prefix = "Hello, World!";
		ASSERT_FALSE(string::StartsWith(line, prefix));
	}

	TEST(StringUtils, TestEmptyStringEndsWithSuffix) {
		constexpr auto* line = "";
		constexpr auto* prefix = "Hello";
		ASSERT_FALSE(string::EndsWith(line, prefix));
	}

	TEST(StringUtils, TestStringEndsWithSuffix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "World!";
		ASSERT_TRUE(string::EndsWith(line, prefix));
	}

	TEST(StringUtils, TestStringDoesNotEndWithSuffix) {
		constexpr auto* line = "Hello, World!";
		constexpr auto* prefix = "Hello";
		ASSERT_FALSE(string::EndsWith(line, prefix));
	}

	TEST(StringUtils, TestShortStringDoesNotEndWithLongSuffix) {
		constexpr auto* line = "Hello";
		constexpr auto* prefix = "Hello, World!";
		ASSERT_FALSE(string::EndsWith(line, prefix));
	}

	TEST(StringUtils, TestSplitEmptyString) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("", delimiter);
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringUtils, TestSplitWhitespaceString) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("  ", delimiter);
		ASSERT_TRUE(tokens.empty());
	}
	TEST(StringUtils, TestSplitStringOnWhitespace) {
		constexpr auto* delimiter = " ";
		const auto tokens = string::Split("   vt 0.707 0.395 0.684   ", delimiter);
		ASSERT_EQ(tokens.size(), 4);
		ASSERT_EQ("vt", tokens[0]);
		ASSERT_EQ("0.707", tokens[1]);
		ASSERT_EQ("0.395", tokens[2]);
		ASSERT_EQ("0.684", tokens[3]);
	}
}