#include <gtest/gtest.h>

#include "object_loader.hpp"

class ObjectLoaderTest : public testing::Test {
protected:
	template <typename T>
	T ParseToken(const std::string_view token) const {
		return ObjectLoader::ParseToken<T>(token);
	}

	template <typename T, std::size_t N>
	std::array<T, N> ParseLine(const std::string_view line) const {
		return ObjectLoader::ParseLine<T, N>(line);
	}
};

namespace {

	TEST_F(ObjectLoaderTest, TestParseEmptyStringToken) {
		ASSERT_THROW(ParseToken<int>(""), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseIntToken) {
		const auto expected = 42;
		const auto actual = ParseToken<int>("42");
		ASSERT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestParseFloatToken) {
		const auto expected = 3.14f;
		const auto actual = ParseToken<float>("3.14");
		ASSERT_FLOAT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestParseInvalidFloat) {
		ASSERT_THROW(ParseToken<float>("Definitely a float"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseEmptyLine) {
		ASSERT_THROW((ParseLine<float, 0>("")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithIncorrectSizeParameter) {
		ASSERT_THROW((ParseLine<float, 4>("vt 0.707 0.395 0.684")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithCorrectSizeParameter) {
		const auto tokens = ParseLine<float, 3>("vt 0.707 0.395 0.684");
		ASSERT_EQ(3, tokens.size());
		ASSERT_FLOAT_EQ(0.707f, tokens[0]);
		ASSERT_FLOAT_EQ(0.395f, tokens[1]);
		ASSERT_FLOAT_EQ(0.684f, tokens[2]);
	}
}
