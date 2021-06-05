#include <gtest/gtest.h>

#include "geometry/mesh_simplifier.cpp"

using namespace geometry;

namespace {
	TEST(MeshSimplifierTest, TestAverageVertices) {
		const Vertex v0{0, glm::vec3{1.f}, glm::vec3{2.f}};
		const Vertex v1{1, glm::vec3{3.f}, glm::vec3{4.f}};
		const auto average = AverageVertices(2, v0, v1);
		ASSERT_EQ(2, average->Id());
		ASSERT_EQ(glm::vec3{2.f}, average->Position());
		ASSERT_EQ(glm::vec3{3.f}, average->Normal());
	}
}
