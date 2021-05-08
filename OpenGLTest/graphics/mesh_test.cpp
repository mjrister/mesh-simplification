#include "graphics/mesh.hpp"

#include <gtest/gtest.h>

TEST(Mesh, TestCreateMeshWithNoPositions) {
	const std::vector<GLfloat> positions;
	ASSERT_THROW(Mesh{positions}, std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithNon3DPositions) {
	const std::vector positions2d{0.0f, 0.1f};
	const std::vector positions4d{0.0f, 0.1f, 0.2f, 0.3f};
	ASSERT_THROW(Mesh{positions2d}, std::invalid_argument);
	ASSERT_THROW(Mesh{positions4d}, std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithNon2DTextureCoordinates) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f};
	const std::vector texture_coordinates3d{0.0f, 0.1f, 0.2f};
	const std::vector texture_coordinates4d{0.0f, 0.1f, 0.2f, 0.3f};
	ASSERT_THROW((Mesh{positions3d, texture_coordinates3d}), std::invalid_argument);
	ASSERT_THROW((Mesh{positions3d, texture_coordinates4d}), std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithNon3DNormals) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f};
	const std::vector normals2d{0.0f, 0.1f};
	const std::vector normals4d{0.0f, 0.1f, 0.2f, 0.3f};
	ASSERT_THROW((Mesh{positions3d, {}, normals2d}), std::invalid_argument);
	ASSERT_THROW((Mesh{positions3d, {}, normals4d}), std::invalid_argument);
}

TEST(Mesh, TestCreateNonTriangleMesh) {
	const std::vector positions3{0.0f, 0.1f, 0.2f};
	const std::vector<GLuint> indices2d{0, 1};
	const std::vector<GLuint> indices4d{0, 1, 2, 3};
	ASSERT_THROW((Mesh{positions3, {}, {}, indices2d}), std::invalid_argument);
	ASSERT_THROW((Mesh{positions3, {}, {}, indices4d}), std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithMissingTextureCoordinates) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f, 1.0f, 1.1f, 1.2f};
	const std::vector texture_coordinates2d{0.0f, 0.1f};
	ASSERT_THROW((Mesh{positions3d, texture_coordinates2d}), std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithExtraTextureCoordinates) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f};
	const std::vector texture_coordinates2d{0.0f, 0.1f, 0.2f, 0.3f};
	ASSERT_THROW((Mesh{positions3d, texture_coordinates2d}), std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithMissingNormals) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f, 1.0f, 1.1f, 1.2f};
	const std::vector normals3d{0.0f, 0.1f, 0.2f};
	ASSERT_THROW((Mesh{positions3d, {}, normals3d}), std::invalid_argument);
}

TEST(Mesh, TestCreateMeshWithExtraNormals) {
	const std::vector positions3d{0.0f, 0.1f, 0.2f};
	const std::vector normals3d{ 0.0f, 0.1f, 0.2f, 1.0f, 1.1f, 1.2f};
	ASSERT_THROW((Mesh{positions3d, {}, normals3d}), std::invalid_argument);
}