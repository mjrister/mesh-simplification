#include "graphics/mesh.cpp"

#include <vector>

#include <gtest/gtest.h>

using namespace gfx;
using namespace glm;
using namespace std;

namespace {

TEST(MeshTest, TestValidateMeshWithInvalidNumberOfPositions) {
	for (auto i = 0; i <= 4; ++i) {
		if (i != 3) {
			ASSERT_THROW((Mesh{vector<vec3>(i), {}, {}, {}}), invalid_argument);
		}
	}
}

TEST(MeshTest, TestValidateMeshWithInvalidNumberOfTextureCoordinates) {
	const vector<vec3> positions(3);
	for (auto i = 1; i <= 4; ++i) {
		if (i != 3) {
			ASSERT_THROW((Mesh{positions, vector<vec2>(i), {}, {}}), invalid_argument);
		}
	}
}

TEST(MeshTest, TestValidateMeshInvalidNumberOfNormals) {
	const vector<vec3> positions(3);
	for (auto i = 1; i <= 4; ++i) {
		if (i != 3) {
			ASSERT_THROW((Mesh{positions, {}, vector<vec3>(i), {}}), invalid_argument);
		}
	}
}

TEST(MeshTest, TestValidateMeshWithInvalidIndices) {
	const vector<vec3> positions(3);
	for (auto i = 1; i <= 4; ++i) {
		if (i != 3) {
			ASSERT_THROW((Mesh{positions, {}, {}, vector<GLuint>(i)}), invalid_argument);
		}
	}
}

TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesAndNormals) {
	const vector<vec3> positions(3);
	const vector<vec2> texture_coordinates(3);
	const vector<vec3> normals(3);
	ASSERT_NO_THROW((Mesh{positions, texture_coordinates, normals, {}}));
}

TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesNormalsAndIndices) {
	const vector<vec3> positions(4);
	const vector<vec2> texture_coordinates(2);
	const vector<vec3> normals(5);
	const vector<GLuint> indices(3);
	ASSERT_NO_THROW((Mesh{positions, texture_coordinates, normals, indices}));
}
}
