#include "graphics/object_loader.hpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>

namespace gfx {

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

		static constexpr auto npos_index_ = ObjectLoader::npos_index_;
	};
}

namespace {
	using namespace gfx;

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
		ASSERT_EQ((glm::ivec3{0, npos_index_, npos_index_}), ParseIndexGroup("1"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((glm::ivec3{0, 1, npos_index_}), ParseIndexGroup("1/2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{0, npos_index_, 1}), ParseIndexGroup("1//2"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{0, 1, 2}), ParseIndexGroup("1/2/3"));
	}

	TEST_F(ObjectLoaderTest, TestParseIndexGroupWithInvalidFormat) {
		ASSERT_THROW(ParseIndexGroup(""), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1//"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//3"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/2/"), std::invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/3"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestParseFaceWithCorrectNumberOfIndexGroups) {
		ASSERT_EQ((std::array<glm::ivec3, 3>{
			glm::ivec3{0, 1, 2},
			glm::ivec3{3, 4, 5},
			glm::ivec3{6, 7, 8}
		}), ParseFace("f 1/2/3 4/5/6 7/8/9"));
	}

	TEST_F(ObjectLoaderTest, TestParseFaceWithIncorrectNumberOfIndexGroups) {
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6"), std::invalid_argument);
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6 7/8/9 10/11/12"), std::invalid_argument);
	}

	TEST_F(ObjectLoaderTest, TestLoadMeshWithoutFaces) {

		std::istringstream ss{R"(
			# positions
			v 0.0  1.0 0.0
			v 0.5  1.0 0.0
			v 1.0  1.0 0.0
			# texture coordinates
			vt 0.0  0.0
			vt 0.5  0.0
			vt 1.0  0.0
			# normals
			vn 0.0 0.1 0.2
			vn 1.0 1.1 1.2
			vn 2.0 2.1 2.2
		)"};

		constexpr glm::vec3 v0{0.0f, 1.0f, 0.0f};
		constexpr glm::vec3 v1{0.5f, 1.0f, 0.0f};
		constexpr glm::vec3 v2{1.0f, 1.0f, 0.0f};

		constexpr glm::vec2 vt0{0.0f, 0.0f};
		constexpr glm::vec2 vt1{0.5f, 0.0f};
		constexpr glm::vec2 vt2{1.0f, 0.0f};

		constexpr glm::vec3 vn0{0.0f, 0.1f, 0.2f};
		constexpr glm::vec3 vn1{1.0f, 1.1f, 1.2f};
		constexpr glm::vec3 vn2{2.0f, 2.1f, 2.2f};

		const auto mesh = ObjectLoader::LoadMesh(ss);

		ASSERT_EQ((std::vector{v0, v1, v2}), mesh.Positions());
		ASSERT_EQ((std::vector{vt0, vt1, vt2}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector{vn0, vn1, vn2}), mesh.Normals());
		ASSERT_TRUE(mesh.Indices().empty());
	}

	TEST_F(ObjectLoaderTest, TestLoadMeshWithFaces) {

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
			f 1/1/3 2/2/2 6/4/1
			f 2/2/2 3/3/1 5/5/2
			f 2/2/2 4/5/2 6/6/1
		)"};

		constexpr glm::vec3 v0{0.0f, 1.0f, 0.0f};
		constexpr glm::vec3 v1{0.5f, 1.0f, 0.0f};
		constexpr glm::vec3 v2{1.0f, 1.0f, 0.0f};
		constexpr glm::vec3 v3{0.75f, 0.5f, 0.0f};
		constexpr glm::vec3 v4{0.5f, 0.0f, 0.0f};
		constexpr glm::vec3 v5{0.25f, 0.5f, 0.0f};

		constexpr glm::vec2 vt0{0.0f, 0.0f};
		constexpr glm::vec2 vt1{0.5f, 0.0f};
		constexpr glm::vec2 vt2{1.0f, 0.0f};
		constexpr glm::vec2 vt3{0.25f, 0.5f};
		constexpr glm::vec2 vt4{0.75f, 0.5f};
		constexpr glm::vec2 vt5{0.5f, 1.0f};

		constexpr glm::vec3 vn0{0.0f, 0.1f, 0.2f};
		constexpr glm::vec3 vn1{1.0f, 1.1f, 1.2f};
		constexpr glm::vec3 vn2{2.0f, 2.1f, 2.2f};

		const auto mesh = ObjectLoader::LoadMesh(ss);

		ASSERT_EQ((std::vector{v0, v1, v5, v1, v2, v4, v1, v3, v5 }), mesh.Positions());
		ASSERT_EQ((std::vector{vt0, vt1, vt3, vt1, vt2, vt4, vt1, vt4, vt5}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector{vn2, vn1, vn0, vn1, vn0, vn1, vn1, vn1, vn0}), mesh.Normals());
		ASSERT_TRUE(mesh.Indices().empty());
	}
}
