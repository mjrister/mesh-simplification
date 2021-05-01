#include "object_loader.hpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>

class ObjectLoaderTest : public testing::Test {
protected:
	template <typename T>
	static auto ParseToken(const std::string_view token) {
		return ObjectLoader::ParseToken<T>(token);
	}

	template <typename T, std::uint8_t N>
	static auto ParseLine(const std::string_view line) {
		return ObjectLoader::ParseLine<T, N>(line);
	}

	static auto ParseIndexGroup(const std::string_view line) {
		return ObjectLoader::ParseIndexGroup(line);
	}

	static auto ParseFace(const std::string_view line) {
		return ObjectLoader::ParseFace(line);
	}

	static void ValidateIndex(const GLint index, const GLint max_value) {
		ObjectLoader::ValidateIndex(index, max_value);
	}

	template <typename T, std::uint8_t N>
	static auto Flatten(const std::vector<glm::vec<N, T>>& tuples) {
		return ObjectLoader::Flatten(tuples);
	}

	static auto GetPositionIndices(const std::vector<std::array<glm::uvec3, 3>>& faces) {
		return ObjectLoader::GetPositionIndices(faces);
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

	TEST_F(ObjectLoaderTest, TestParseLineWithThreeInts) {
		ASSERT_EQ((glm::uvec3{0, 1, 2}), (ParseLine<GLuint, 3>("f 0 1 2")));
	}


	TEST_F(ObjectLoaderTest, TestParseLineWithFourFloats) {
		ASSERT_EQ((glm::vec4{0.707f, 0.395f, 0.684f, 0.967f}), (ParseLine<GLfloat, 4>("vt 0.707 0.395 0.684 0.967f")));
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
		constexpr std::array<glm::uvec3, 3> expected{
			glm::uvec3{0, 1, 2},
			glm::uvec3{3, 4, 5},
			glm::uvec3{6, 7, 8}
		};
		const auto actual = ParseFace("f 0/1/2 3/4/5 6/7/8");
		ASSERT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestParseFaceWithIncorrectNumberOfIndexGroups) {
		ASSERT_THROW(ParseFace("f 0/1/2 3/4/5"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestValidateIndex) {
		constexpr auto max_value = 42;
		ASSERT_NO_THROW(ValidateIndex(0, max_value));
		ASSERT_NO_THROW(ValidateIndex(42, max_value));
		ASSERT_THROW(ValidateIndex(-1, max_value), std::invalid_argument);
		ASSERT_THROW(ValidateIndex(43, max_value), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestFlattenVec3) {
		const std::vector expected{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f};
		const auto actual = Flatten(std::vector<glm::vec3>{
			{0.f, 1.f, 2.f},
			{3.f, 4.f, 5.f},
			{6.f, 7.f, 8.f}
		});
		ASSERT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestFlattenVec2) {
		const std::vector expected{0.f, 1.f, 2.f, 3.f, 4.f, 5.f};
		const auto actual = Flatten(std::vector<glm::vec2>{
			{0.f, 1.f},
			{2.f, 3.f},
			{4.f, 5.f}
		});
		ASSERT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestGetPositionIndices) {

		constexpr std::array<glm::uvec3, 3> face0{
			glm::uvec3{0, npos_index, npos_index},
			glm::uvec3{1, npos_index, npos_index},
			glm::uvec3{2, npos_index, npos_index}
		};
		constexpr std::array<glm::uvec3, 3> face1{
			glm::uvec3{3, npos_index, npos_index},
			glm::uvec3{4, npos_index, npos_index},
			glm::uvec3{5, npos_index, npos_index}
		};
		constexpr std::array<glm::uvec3, 3> face2{
			glm::uvec3{6, npos_index, npos_index},
			glm::uvec3{7, npos_index, npos_index},
			glm::uvec3{8, npos_index, npos_index}
		};

		const std::vector expected{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
		const auto actual = GetPositionIndices({face0, face1, face2});

		ASSERT_EQ(expected, actual);
	}
}
