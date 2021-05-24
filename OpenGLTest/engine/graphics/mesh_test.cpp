#include <vector>

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "engine/graphics/mesh.cpp"

namespace {
	using namespace gfx;

	TEST(MeshTest, TestValidateMeshWithNoPositions) {
		const std::vector<glm::vec4> positions;
		ASSERT_THROW(Validate(positions, {}, {}, {}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithInvalidTextureCoordinates) {
		const std::vector<glm::vec4> positions{{0.f, .1f, .2f, .3f}, {1.f, 1.1f, 1.2f, 1.3f}};
		ASSERT_THROW(Validate(positions, {{0.f, .1f}}, {}, {}), std::invalid_argument);
		ASSERT_THROW(Validate(positions, {{0.f, 0.1f}, {1.f, 1.1f}, {2.f, 2.1f}}, {}, {}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshInvalidNormals) {
		const std::vector<glm::vec4> positions{{0.f, .1f, .2f, .3f}, {1.f, 1.1f, 1.2f, 1.3f}};
		ASSERT_THROW(Validate(positions, {}, {{0.f, .1f, .2f}}, {}), std::invalid_argument);
		ASSERT_THROW(Validate(positions, {}, {{0.f, .1f, .2f}, {1.f, 1.1f, 1.2f}, {2.f, 2.1f, 2.2f}}, {}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithInvalidIndices) {
		const std::vector<glm::vec4> positions{{0.f, .1f, .2f, .3f}};
		ASSERT_THROW(Validate(positions, {}, {}, {0, 1}), std::invalid_argument);
		ASSERT_THROW(Validate(positions, {}, {}, {0, 1, 2, 3}), std::invalid_argument);
	}

	TEST(MeshTest, TestValidateMeshWithCorrectPositionsTextureCoordinatesNormalsAndIndices) {
		const std::vector<glm::vec4> positions{{0.f, .1f, .2f, .3f}, {1.f, 1.1f, 1.2f, 1.3f}, {2.f, 2.1f, 2.2f, 2.3f}};
		const std::vector<glm::vec2> texture_coordinates{{0.f, 0.1f}, {1.f, 1.1f}, {2.f, 2.1f}};
		const std::vector<glm::vec3> normals{{0.f, .1f, .2f}, {1.f, 1.1f, 1.2f}, {2.f, 2.1f, 2.2f}};
		const std::vector<GLuint> indices{0, 1, 2};
		ASSERT_NO_THROW(Validate(positions, texture_coordinates, normals, indices));
	}
}
