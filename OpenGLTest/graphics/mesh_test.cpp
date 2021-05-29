#include <vector>

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "graphics/mesh.cpp"

namespace {

	TEST(MeshTest, TestValidateMeshWithInvalidNumberOfPositions) {
		for (std::uint8_t i = 0; i <= static_cast<std::uint8_t>(4); ++i) {
			if (i != 3) {
				ASSERT_THROW(Validate(std::vector<glm::vec4>(i), {}, {}, {}), std::invalid_argument);
			}
		}
	}

	TEST(MeshTest, TestValidateMeshWithInvalidNumberOfTextureCoordinates) {
		const std::vector<glm::vec4> positions(3);
		for (std::uint8_t i = 1; i <= static_cast<std::uint8_t>(4); ++i) {
			if (i != 3) {
				ASSERT_THROW(Validate(positions, std::vector<glm::vec2>(i), {}, {}), std::invalid_argument);
			}
		}
	}

	TEST(MeshTest, TestValidateMeshInvalidNumberOfNormals) {
		const std::vector<glm::vec4> positions(3);
		for (std::uint8_t i = 1; i <= static_cast<std::uint8_t>(4); ++i) {
			if (i != 3) {
				ASSERT_THROW(Validate(positions, {}, std::vector<glm::vec3>(i), {}), std::invalid_argument);
			}
		}
	}

	TEST(MeshTest, TestValidateMeshWithInvalidIndices) {
		const std::vector<glm::vec4> positions(3);
		for (std::uint8_t i = 1; i <= static_cast<std::uint8_t>(4); ++i) {
			if (i != 3) {
				ASSERT_THROW(Validate(positions, {}, {}, std::vector<GLuint>(i)), std::invalid_argument);
			}
		}
	}

	TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesAndNormals) {
		const std::vector<glm::vec4> positions(3);
		const std::vector<glm::vec2> texture_coordinates(3);
		const std::vector<glm::vec3> normals(3);
		ASSERT_NO_THROW(Validate(positions, texture_coordinates, normals, {}));
	}

	TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesNormalsAndIndices) {
		const std::vector<glm::vec4> positions(4);
		const std::vector<glm::vec2> texture_coordinates(2);
		const std::vector<glm::vec3> normals(5);
		const std::vector<GLuint> indices(3);
		ASSERT_NO_THROW(Validate(positions, texture_coordinates, normals, indices));
	}
}
