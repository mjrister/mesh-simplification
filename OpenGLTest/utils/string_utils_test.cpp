#include <gtest/gtest.h>

#include "utils/string_utils.hpp"

using namespace utils;

namespace {

	TEST(StringUtils, TestEmptyStringStartsWithPrefix) {
		ASSERT_FALSE(StartsWith("", "Hello, World!"));
	}

	TEST(StringUtils, TestStringStartsWithPrefix) {
		ASSERT_TRUE(StartsWith("Hello, World!", "Hello"));
	}

	TEST(StringUtils, TestStringDoesNotStartWithPrefix) {
		ASSERT_FALSE(StartsWith("Hello, World!", "World"));
	}

	TEST(StringUtils, TestShortStringDoesNotContainLongPrefix) {
		ASSERT_FALSE(StartsWith("Hello", "Hello, World!"));
	}

	TEST(StringUtils, TestSplitEmptyString) {
		constexpr auto* delimiter = " ";
		const auto tokens = Split("", delimiter);
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringUtils, TestSplitWhitespaceString) {
		constexpr auto* delimiter = " ";
		const auto tokens = Split("  ", delimiter);
		ASSERT_TRUE(tokens.empty());
	}
	TEST(StringUtils, TestSplitStringOnWhitespace) {
		constexpr auto* delimiter = " ";
		const auto tokens = Split("   vt 0.707 0.395 0.684   ", delimiter);
		ASSERT_EQ(tokens.size(), 4);
		ASSERT_EQ(tokens[0], "vt");
		ASSERT_EQ(tokens[1], "0.707");
		ASSERT_EQ(tokens[2], "0.395");
		ASSERT_EQ(tokens[3], "0.684");
	}
}