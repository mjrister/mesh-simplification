#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "engine/graphics/mesh.h"
#include "engine/graphics/obj_loader.h"

namespace {
	using namespace gfx;

	void AssertLoadMeshThrowsWithInvalidArgument(const std::string_view obj_source) {
		std::istringstream ss{obj_source.data()};
		ASSERT_THROW(obj_loader::LoadMesh(ss), std::invalid_argument);
	}

	TEST(ObjLoaderTest, TestLoadEmptyMeshThrowsAnException) {
		AssertLoadMeshThrowsWithInvalidArgument("");
	}

	TEST(ObjLoaderTest, TestLoadMeshWithInvalidPosition) {
		AssertLoadMeshThrowsWithInvalidArgument("v ");
		AssertLoadMeshThrowsWithInvalidArgument("v 0.0 0.1");
		AssertLoadMeshThrowsWithInvalidArgument("v 0.0 0.1 0.2 0.3");
		AssertLoadMeshThrowsWithInvalidArgument("v a b c");
	}

	TEST(ObjLoaderTest, TestLoadMeshWithInvalidTextureCoordinate) {
		AssertLoadMeshThrowsWithInvalidArgument("vt ");
		AssertLoadMeshThrowsWithInvalidArgument("vt 0.0");
		AssertLoadMeshThrowsWithInvalidArgument("vt 0.0 0.1 0.2");
		AssertLoadMeshThrowsWithInvalidArgument("vt a b");
	}

	TEST(ObjLoaderTest, TestLoadMeshWithInvalidNormal) {
		AssertLoadMeshThrowsWithInvalidArgument("vn ");
		AssertLoadMeshThrowsWithInvalidArgument("vn 0.0 0.1");
		AssertLoadMeshThrowsWithInvalidArgument("vn 0.0 0.1 0.2 0.3");
		AssertLoadMeshThrowsWithInvalidArgument("vn a b c");
	}

	TEST(ObjLoaderTest, TestLoadMeshWithInvalidFace) {
		AssertLoadMeshThrowsWithInvalidArgument("f ");
		AssertLoadMeshThrowsWithInvalidArgument("f 0 1");
		AssertLoadMeshThrowsWithInvalidArgument("f 0 1 2 3");
		AssertLoadMeshThrowsWithInvalidArgument("f a b c");
		AssertLoadMeshThrowsWithInvalidArgument("f 0/a 1/b 1/c");
		AssertLoadMeshThrowsWithInvalidArgument("f 0//a 1//b 2//c");
		AssertLoadMeshThrowsWithInvalidArgument("f 0/1/a 2/3/b 4/5/c");
		AssertLoadMeshThrowsWithInvalidArgument("f / / /");
		AssertLoadMeshThrowsWithInvalidArgument("f // // //");
		AssertLoadMeshThrowsWithInvalidArgument("f 0/ 0/ 0/");
		AssertLoadMeshThrowsWithInvalidArgument("f /1 /1 /1");
		AssertLoadMeshThrowsWithInvalidArgument("f 0/1/ 0/1/ 0/1/");
		AssertLoadMeshThrowsWithInvalidArgument("f /1/2 /1/2 /1/2");
	}

	TEST(ObjLoaderTest, TestLoadMeshWithPositions) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 v0{0.0f, 0.1f, 0.2f}, v1{1.0f, 1.1f, 1.2f}, v2{2.0f, 2.1f, 2.2f};
		ASSERT_EQ((std::vector{v0, v1, v2}), mesh.Positions());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithTextureCoordinates) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			vt 3.0 3.1
			vt 4.0 4.1
			vt 5.0 5.1
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec2 vt0{3.0f, 3.1f}, vt1{4.0f, 4.1f}, vt2{5.0f, 5.1f};
		ASSERT_EQ((std::vector{vt0, vt1, vt2}), mesh.TextureCoordinates());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithNormals) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			vn 6.0 6.1 6.2
			vn 7.0 7.1 7.2
			vn 8.0 8.1 8.2
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 vn0{6.0f, 6.1f, 6.2f}, vn1{7.0f, 7.1f, 7.2f}, vn2{8.0f, 8.1f, 8.2f};
		ASSERT_EQ((std::vector{vn0, vn1, vn2}), mesh.Normals());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithPositionIndices) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			v 3.0 3.1 3.2
			f 1 2 3
			f 1 2 4
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 v0{0.0f, 0.1f, 0.2f}, v1{1.0f, 1.1f, 1.2f}, v2{2.0f, 2.1f, 2.2f}, v3{3.0f, 3.1f, 3.2f};
		ASSERT_EQ((std::vector{v0, v1, v2, v0, v1, v3}), mesh.Positions());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithPositionAndTextureCoordinateIndices) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			v 3.0 3.1 3.2
			vt 4.0 4.1
			vt 5.0 5.1
			vt 6.0 6.1
			vt 7.0 7.1
			f 1/1 2/2 3/4
			f 1/1 2/2 4/3
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 v0{0.0f, 0.1f, 0.2f}, v1{1.0f, 1.1f, 1.2f}, v2{2.0f, 2.1f, 2.2f}, v3{3.0f, 3.1f, 3.2f};
		constexpr glm::vec2 vt0{4.0f, 4.1f}, vt1{5.0f, 5.1f}, vt2{6.0f, 6.1f}, vt3{7.0f, 7.1f};
		ASSERT_EQ((std::vector{v0, v1, v2, v0, v1, v3}), mesh.Positions());
		ASSERT_EQ((std::vector{vt0, vt1, vt3, vt0, vt1, vt2}), mesh.TextureCoordinates());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithPositionAndNormalIndices) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			v 3.0 3.1 3.2
			vn 8.0  8.1  8.2
			vn 9.0  9.1  9.2
			vn 10.0 10.1 10.2
			f 1//1 2//2 3//3
			f 1//3 2//2 4//1
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 v0{0.0f, 0.1f, 0.2f}, v1{1.0f, 1.1f, 1.2f}, v2{2.0f, 2.1f, 2.2f}, v3{3.0f, 3.1f, 3.2f};
		constexpr glm::vec3 vn0{8.0f, 8.1f, 8.2f}, vn1{9.0f, 9.1f, 9.2f}, vn2{10.0f, 10.1f, 10.2f};
		ASSERT_EQ((std::vector{v0, v1, v2, v0, v1, v3}), mesh.Positions());
		ASSERT_EQ((std::vector{vn0, vn1, vn2, vn2, vn1, vn0}), mesh.Normals());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithPositionTextureCoordinatesAndNormalIndices) {
		std::istringstream ss{R"(
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			v 3.0 3.1 3.2
			vt 4.0 4.1
			vt 5.0 5.1
			vt 6.0 6.1
			vt 7.0 7.1
			vn 8.0  8.1  8.2
			vn 9.0  9.1  9.2
			vn 10.0 10.1 10.2
			f 1/1/1 2/2/2 3/4/3
			f 1/1/3 2/2/2 4/3/1
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		constexpr glm::vec3 v0{0.0f, 0.1f, 0.2f}, v1{1.0f, 1.1f, 1.2f}, v2{2.0f, 2.1f, 2.2f}, v3{3.0f, 3.1f, 3.2f};
		constexpr glm::vec2 vt0{4.0f, 4.1f}, vt1{5.0f, 5.1f}, vt2{6.0f, 6.1f}, vt3{7.0f, 7.1f};
		constexpr glm::vec3 vn0{8.0f, 8.1f, 8.2f}, vn1{9.0f, 9.1f, 9.2f}, vn2{10.0f, 10.1f, 10.2f};
		ASSERT_EQ((std::vector{v0, v1, v2, v0, v1, v3}), mesh.Positions());
		ASSERT_EQ((std::vector{vt0, vt1, vt3, vt0, vt1, vt2}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector{vn0, vn1, vn2, vn2, vn1, vn0}), mesh.Normals());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithComments) {
		std::istringstream ss{R"(
			# position
			v 0.0 0.1 0.2
			# texture coordinates
			# vt 1.1 1.0
			# normals
			vn 2.0 2.1 2.2
		)"};
		const auto mesh = obj_loader::LoadMesh(ss);
		ASSERT_EQ((std::vector<glm::vec3>{{0.0f, 0.1f, 0.2f}}), mesh.Positions());
		ASSERT_EQ((std::vector<glm::vec2>{}), mesh.TextureCoordinates());
		ASSERT_EQ((std::vector<glm::vec3>{{2.0f, 2.1f, 2.2f}}), mesh.Normals());
	}
}
