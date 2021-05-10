#include <vector>

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "graphics/mesh.h"

using namespace gfx;

namespace {

	TEST(Mesh, TestCreateMeshWithNoVertexPositions) {
		const std::vector<glm::vec3> positions;
		ASSERT_THROW(Mesh{positions}, std::invalid_argument);
	}

	TEST(Mesh, TestCreateTriangleMeshWithMissingIndices) {
		const std::vector<glm::vec3> positions{ {0.0f, 0.1f, 0.2f} };
		ASSERT_THROW((Mesh{positions, {}, {}, {0, 1}}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateTriangleMeshWithExtraIndices) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}};
		ASSERT_THROW((Mesh{positions, {}, {}, {0, 1, 2, 3}}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateMeshWithMissingTextureCoordinates) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}};
		const std::vector<glm::vec2> texture_coordinates{{0.0f, 0.1f}};
		ASSERT_THROW((Mesh{positions, texture_coordinates}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateMeshWithExtraTextureCoordinates) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}};
		const std::vector<glm::vec2> texture_coordinates{{0.0f, 0.1f}, {0.2f, 0.3f}};
		ASSERT_THROW((Mesh{positions, texture_coordinates}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateMeshWithMissingNormals) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}};
		const std::vector<glm::vec3> normals{{0.0f, 0.1f, 0.2f}};
		ASSERT_THROW((Mesh{positions, {}, normals}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateMeshWithExtraNormals) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}};
		const std::vector<glm::vec3> normals{{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}};
		ASSERT_THROW((Mesh{positions, {}, normals}), std::invalid_argument);
	}

	TEST(Mesh, TestCreateMeshWithPositionsTextureCoordinatesNormalsAndIndices) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1, 0.2f}, {1.0f, 1.1f, 1.2f}, {2.0, 2.1f, 2.2f}};
		const std::vector<glm::vec2> texture_coordinates{{0.0f, 0.1f}, {1.0f, 1.1f}, {2.0f, 2.1f}};
		const std::vector<glm::vec3> normals{{0.0f, 0.1, 0.2f}, {1.0f, 1.1f, 1.2f}, {2.0, 2.1f, 2.2f}};
		const std::vector<GLuint> indices{0, 1, 2};
		ASSERT_NO_THROW((Mesh{positions, texture_coordinates, normals, indices}));
	}
}
