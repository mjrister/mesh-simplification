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
	auto ParseLine(const std::string_view line) const {
		return ObjectLoader::ParseLine<T, N>(line);
	}

	glm::uvec3 ParseIndexGroup(const std::string_view line) const {
		return ObjectLoader::ParseIndexGroup(line);
	}

	std::array<glm::uvec3, 3> ParseFace(const std::string_view line) const {
		return ObjectLoader::ParseFace(line);
	}

	void ValidateIndex(const GLint index, const GLint max_value) const {
		return ObjectLoader::ValidateIndex(index, max_value);
	}

	static constexpr GLuint npos_index = ObjectLoader::npos_index_;
};

namespace {

	TEST_F(ObjectLoaderTest, TestParseEmptyStringToken) {
		ASSERT_THROW(ParseToken<GLuint>(""), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseIntToken) {
		ASSERT_EQ(42, ParseToken<GLuint>("42"));
	}

	TEST_F(ObjectLoaderTest, TestParseFloatToken) {
		ASSERT_FLOAT_EQ(3.14f, ParseToken<GLfloat>("3.14"));
	}

	TEST_F(ObjectLoaderTest, TestParseInvalidFloat) {
		ASSERT_THROW(ParseToken<GLfloat>("Definitely a float"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseEmptyLine) {
		ASSERT_THROW((ParseLine<GLfloat, 3>("")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithInvalidSizeTemplateArgument) {
		ASSERT_THROW((ParseLine<GLfloat, 2>("vt 0.707 0.395 0.684")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithTwoInts) {
		ASSERT_EQ((glm::uvec2{0, 1}), (ParseLine<GLuint, 2>("vt 0 1")));
	}


	TEST_F(ObjectLoaderTest, TestParseLineWithThreeFloats) {
		ASSERT_EQ((glm::vec3{0.707f, 0.395f, 0.684f}), (ParseLine<GLfloat, 3>("vt 0.707 0.395 0.684")));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionIndex) {
		ASSERT_EQ((glm::uvec3{1, npos_index, npos_index}), ParseIndexGroup("1"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((glm::uvec3{1, 2, npos_index}), ParseIndexGroup("1/2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((glm::uvec3{1, npos_index, 2}), ParseIndexGroup("1//2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
		ASSERT_EQ((glm::uvec3{1, 2, 3}), ParseIndexGroup("1/2/3"));
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

	TEST_F(ObjectLoaderTest, TestParseFaceWithCorrectNumberOfIndexGroups) {
		ASSERT_EQ((std::array<glm::uvec3, 3>{
			glm::uvec3{0, 1, 2},
			glm::uvec3{3, 4, 5},
			glm::uvec3{6, 7, 8}
		}), ParseFace("f 0/1/2 3/4/5 6/7/8"));
	}

	TEST_F(ObjectLoaderTest, TestParseFaceWithIncorrectNumberOfIndexGroups) {
		ASSERT_THROW(ParseFace("f 0/1/2 3/4/5"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestValidateIndex) {
		constexpr auto min_value = 0, max_value = 42;
		ASSERT_NO_THROW(ValidateIndex(min_value, max_value));
		ASSERT_NO_THROW(ValidateIndex(max_value, max_value));
		ASSERT_THROW(ValidateIndex(min_value - 1, max_value), std::invalid_argument);
		ASSERT_THROW(ValidateIndex(max_value + 1, max_value), std::invalid_argument);
	}
}
