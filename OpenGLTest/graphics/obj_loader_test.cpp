#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "graphics/obj_loader.cpp"

namespace {
	TEST(ObjLoaderTest, TestParseEmptyToken) {
		ASSERT_THROW(ParseToken<GLint>(""), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseInvalidToken) {
		ASSERT_THROW(ParseToken<GLfloat>("Definitely a float"), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseIntToken) {
		ASSERT_EQ(42, ParseToken<GLint>("42"));
	}

	TEST(ObjLoaderTest, TestParseFloatToken) {
		ASSERT_FLOAT_EQ(3.14f, ParseToken<GLfloat>("3.14"));
	}

	TEST(ObjLoaderTest, TestParseEmptyLine) {
		ASSERT_THROW((ParseLine<GLfloat, 3>("")), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseLineWithInvalidSizeArgument) {
		ASSERT_THROW((ParseLine<GLfloat, 2>("vt 0.707 0.395 0.684")), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseLine) {
		ASSERT_EQ((glm::vec3{.707f, .395f, .684f}), (ParseLine<GLfloat, 3>("vt 0.707 0.395 0.684")));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionIndex) {
		ASSERT_EQ((glm::ivec3{0, npos_index, npos_index}), ParseIndexGroup("1"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((glm::ivec3{0, 1, npos_index}), ParseIndexGroup("1/2"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{0, npos_index, 1}), ParseIndexGroup("1//2"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
		ASSERT_EQ((glm::ivec3{0, 1, 2}), ParseIndexGroup("1/2/3"));
	}

	TEST(ObjLoaderTest, TestParseInvalidIndexGroup) {
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

	TEST(ObjLoaderTest, TestParseFaceWithInvalidNumberOfIndexGroups) {
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6"), std::invalid_argument);
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6 7/8/9 10/11/12"), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseFaceWithThreeIndexGroups) {
		ASSERT_EQ((std::array<glm::ivec3, 3>{
			glm::ivec3{0, 1, 2},
			glm::ivec3{3, 4, 5},
			glm::ivec3{6, 7, 8}
		}), ParseFace("f 1/2/3 4/5/6 7/8/9"));
	}

	TEST(ObjLoaderTest, TestLoadMeshWithoutFaceIndices) {

		std::istringstream ss{R"(
			# positions
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			# texture coordinates
			vt 3.0 3.1
			vt 4.0 4.1
			vt 5.0 5.1
			# normals
			vn 6.0 6.1 6.2
			vn 7.0 7.1 7.2
			vn 8.0 8.1 8.2
		)"};

		const auto mesh = LoadMesh(ss);
		constexpr glm::vec4 v0{0.f, 0.1f, 0.2f, 1.f}, v1{1.f, 1.1f, 1.2f, 1.f}, v2{2.f, 2.1f, 2.2f, 1.f};
		constexpr glm::vec2 vt0{3.f, 3.1f}, vt1{4.f, 4.1f}, vt2{5.f, 5.1f};
		constexpr glm::vec3 vn0{6.f, 6.1f, 6.2f}, vn1{7.f, 7.1f, 7.2f}, vn2{8.f, 8.1f, 8.2f};

		ASSERT_EQ((std::vector{v0, v1, v2}), mesh.Positions());
		ASSERT_EQ((std::vector{vt0, vt1, vt2}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector{vn0, vn1, vn2}), mesh.Normals());
		ASSERT_TRUE(mesh.Indices().empty());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithFaceIndices) {

		std::istringstream ss{R"(
			# positions
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			v 3.0 3.1 3.2
			# texture coordinates
			vt 4.0 4.1
			vt 5.0 5.1
			vt 6.0 6.1
			vt 7.0 7.1
			# normals
			vn 8.0  8.1  8.2
			vn 9.0  9.1  9.2
			vn 10.0 10.1 10.2
			# faces
			f 1/4/2 2/1/3 3/2/1
			f 1/4/2 2/1/3 4/3/1
		)"};

		const auto mesh = LoadMesh(ss);
		constexpr glm::vec4 v0{0.f, .1f, .2f, 1.f}, v1{1.f, 1.1f, 1.2f, 1.f}, v2{2.f, 2.1f, 2.2f, 1.f}, v3{3.f, 3.1f, 3.2f, 1.f};
		constexpr glm::vec2 vt0{4.f, 4.1f}, vt1{5.f, 5.1f}, vt2{6.f, 6.1f}, vt3{7.f, 7.1f};
		constexpr glm::vec3 vn0{8.f, 8.1f, 8.2f}, vn1{9.f, 9.1f, 9.2f}, vn2{10.f, 10.1f, 10.2f};

		ASSERT_EQ((std::vector{v0, v1, v2, v3}), mesh.Positions());
		ASSERT_EQ((std::vector{vt3, vt0, vt1, vt2}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector{vn1, vn2, vn0, vn0}), mesh.Normals());
		ASSERT_EQ((std::vector{0u, 1u, 2u, 0u, 1u, 3u}), mesh.Indices());
	}
}
