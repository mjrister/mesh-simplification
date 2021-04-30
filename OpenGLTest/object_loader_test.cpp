#include "object_loader.hpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>

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

	glm::ivec3 ParseIndexGroup(const std::string_view line) const {
		return ObjectLoader::ParseIndexGroup(line);
	}

	std::array<glm::ivec3, 3> ParseFace(const std::string_view line) const {
		return ObjectLoader::ParseFace(line);
	}
};

namespace {

	TEST_F(ObjectLoaderTest, TestParseEmptyStringToken) {
		ASSERT_THROW(ParseToken<GLint>(""), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseIntToken) {
		ASSERT_EQ(42, ParseToken<GLint>("42"));
	}

	TEST_F(ObjectLoaderTest, TestParseFloatToken) {
		ASSERT_FLOAT_EQ(3.14f, ParseToken<GLfloat>("3.14"));
	}

	TEST_F(ObjectLoaderTest, TestParseInvalidFloat) {
		ASSERT_THROW(ParseToken<GLfloat>("Definitely a float"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseEmptyLine) {
		ASSERT_THROW((ParseLine<GLfloat, 0>("")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithInvalidSizeParameter) {
		ASSERT_THROW((ParseLine<GLfloat, 4>("vt 0.707 0.395 0.684")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithCorrectSizeParameter) {
		ASSERT_EQ((std::array<GLfloat, 3>{0.707f, 0.395f, 0.684f}), (ParseLine<GLfloat, 3>("vt 0.707 0.395 0.684")));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionIndex) {
		ASSERT_EQ((glm::ivec3{1, -1, -1}), ParseIndexGroup("1"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((glm::ivec3{1, 2, -1}), ParseIndexGroup("1/2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{1, -1, 2}), ParseIndexGroup("1//2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionTextureCoordinatesAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{1, 2, 3}), ParseIndexGroup("1/2/3"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithInvalidFormat) {
		ASSERT_THROW(ParseIndexGroup("/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1//"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//3"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/2/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/3"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseFace) {
		constexpr std::array<glm::ivec3, 3> face = {
			glm::ivec3{0, 1, 2},
			glm::ivec3{3, 4, 5},
			glm::ivec3{6, 7, 8}
		};
		ASSERT_EQ(face, ParseFace("f 0/1/2 3/4/5 6/7/8"));
	}

	TEST_F(ObjectLoaderTest, TestParseFaceWithInvalidFormat) {
		ASSERT_THROW(ParseFace("f 0/1/2 3/4/5"), std::invalid_argument);
	}
}
