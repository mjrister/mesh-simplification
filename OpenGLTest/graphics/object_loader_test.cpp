#include "graphics/object_loader.hpp"

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

	static auto GetPositionIndices(const std::vector<std::array<glm::ivec3, 3>>& faces) {
		return ObjectLoader::GetPositionIndices(faces);
	}

	static constexpr auto npos = ObjectLoader::npos_index_;
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
		ASSERT_THROW((ParseLine<GLfloat, 3>("")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithInvalidSizeTemplateArgument) {
		ASSERT_THROW((ParseLine<GLfloat, 2>("vt 0.707 0.395 0.684")), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseLineWithThreeInts) {
		ASSERT_EQ((glm::ivec3{0, 1, 2}), (ParseLine<GLint, 3>("f 0 1 2")));
	}


	TEST_F(ObjectLoaderTest, TestParseLineWithFourFloats) {
		ASSERT_EQ((glm::vec4{0.707f, 0.395f, 0.684f, 0.967f}), (ParseLine<GLfloat, 4>("vt 0.707 0.395 0.684 0.967f")));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionIndex) {
		ASSERT_EQ((glm::ivec3{1, npos, npos}), ParseIndexGroup("1"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((glm::ivec3{1, 2, npos}), ParseIndexGroup("1/2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{1, npos, 2}), ParseIndexGroup("1//2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
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

	TEST_F(ObjectLoaderTest, TestParseFaceWithCorrectNumberOfIndexGroups) {
		constexpr std::array<glm::ivec3, 3> expected{
			glm::ivec3{0, 1, 2},
			glm::ivec3{3, 4, 5},
			glm::ivec3{6, 7, 8}
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
		const std::vector expected{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
		const auto actual = GetPositionIndices({
			{glm::ivec3{0, npos, npos}, glm::ivec3{1, npos, npos}, glm::ivec3{2, npos, npos}},
			{glm::ivec3{3, npos, npos}, glm::ivec3{4, npos, npos}, glm::ivec3{5, npos, npos}},
			{glm::ivec3{6, npos, npos}, glm::ivec3{7, npos, npos}, glm::ivec3{8, npos, npos}}
		});

		ASSERT_EQ(expected, actual);
	}

	TEST_F(ObjectLoaderTest, TestLoadMesh) {

		std::istringstream ss{R"(
			# positions
			v 0.0  1.0 0.0
			v 0.5  1.0 0.0
			v 1.0  1.0 0.0
			v 0.75 0.5 0.0
			v 0.5  0.0 0.0
			v 0.25 0.5 0.0
			# texture coordinates
			vt 0.0  0.0
			vt 0.5  0.0
			vt 1.0  0.0
			vt 0.25 0.5
			vt 0.75 0.5
			vt 0.5  1.0
			# normals
			vn 0.0 0.1 0.2
			vn 1.0 1.1 1.2
			vn 2.0 2.1 2.2
			# faces
			f 0/0/2 1/1/1 5/3/0
			f 1/1/1 2/2/0 3/4/1
			f 1/1/1 3/4/1 5/3/0
			f 5/3/0 3/4/1 4/5/2
		)"};

		const auto mesh = ObjectLoader::LoadMesh(ss);

		const auto& positions = mesh.Positions();
		ASSERT_EQ(positions, (std::vector<GLfloat>{
			0.0f,  1.0f, 0.0f,
			0.5f,  1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,
			0.75f, 0.5f, 0.0f,
			0.5f,  0.0f, 0.0f,
			0.25f, 0.5f, 0.0f
		}));

		const auto& indices = mesh.Indices();
		ASSERT_EQ(indices, (std::vector<GLuint>{
			0, 1, 5,
			1, 2, 3,
			1, 3, 5,
			5, 3, 4
		}));

		const auto& textureCoordinates = mesh.TextureCoordinates();
		ASSERT_EQ(textureCoordinates, (std::vector<GLfloat>{
			0.0f,  0.0f,
			0.5f,  0.0f,
			1.0f,  0.0f,
			0.75f, 0.5f,
			0.5f,  1.0f,
			0.25,  0.5f
		}));

		const auto& normals = mesh.Normals();
		ASSERT_EQ(normals, (std::vector<GLfloat>{
			2.0f, 2.1f, 2.2f,
			1.0f, 1.1f, 1.2f,
			0.0f, 0.1f, 0.2f,
			1.0f, 1.1f, 1.2f,
			2.0f, 2.1f, 2.2f,
			0.0f, 0.1f, 0.2f
		}));
	}
}
