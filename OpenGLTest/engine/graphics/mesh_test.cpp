#include <vector>

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "engine/graphics/mesh.cpp"

namespace {
	using namespace gfx;

	TEST(MeshTest, TestValidateMeshWithNoPositions) {
		const std::vector<glm::vec3> positions;
		ASSERT_THROW(Validate(positions, {}, {}, {}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithInvalidTextureCoordinates) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}};
		ASSERT_THROW(Validate(positions, {{0.0f, 0.1f}}, {}, {}), std::invalid_argument);
		ASSERT_THROW(Validate(positions, {{0.0f, 0.1f}, {0.2f, 0.3f}, {0.4f, 0.5f}}, {}, {}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshInvalidNormals) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}};
		ASSERT_THROW(Validate(positions, {}, {{0.0f, 0.1f, 0.2f}}, {}), std::invalid_argument);
		ASSERT_THROW((Mesh{positions, {}, {{0.0f, 0.1f, 0.2f}, {1.0f, 1.1f, 1.2f}, {2.0f, 2.1f, 2.2f}}}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithInvalidIndices) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1f, 0.2f}};
		ASSERT_THROW(Validate(positions, {}, {}, {0, 1}), std::invalid_argument);
		ASSERT_THROW(Validate(positions, {}, {}, {0, 1, 2, 3}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithPositionsTextureCoordinatesNormalsAndIndices) {
		const std::vector<glm::vec3> positions{{0.0f, 0.1, 0.2f}, {1.0f, 1.1f, 1.2f}, {2.0, 2.1f, 2.2f}};
		const std::vector<glm::vec2> texture_coordinates{{0.0f, 0.1f}, {1.0f, 1.1f}, {2.0f, 2.1f}};
		const std::vector<glm::vec3> normals{{0.0f, 0.1, 0.2f}, {1.0f, 1.1f, 1.2f}, {2.0, 2.1f, 2.2f}};
		const std::vector<GLuint> indices{0, 1, 2};
		ASSERT_NO_THROW((Mesh{positions, texture_coordinates, normals, indices}));
	}
}
