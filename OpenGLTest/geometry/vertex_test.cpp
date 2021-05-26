#include "geometry/vertex.h"

#include <gtest/gtest.h>

namespace {
	using namespace geometry;

	TEST(Vertex, TestGetVertexId) {
		const Vertex vertex{42, glm::vec3{}, glm::vec3{}};
		ASSERT_EQ(42, vertex.Id());
	}

	TEST(Vertex, TestGetVertexPosition) {
		const Vertex vertex{42, glm::vec3{0.f, 1.f, 2.f}, glm::vec3{}};
		ASSERT_EQ((glm::vec3{0.f, 1.f, 2.f}), vertex.Position());
	}

	TEST(Vertex, TestGetVertexNormal) {
		const Vertex vertex{42, glm::vec3{}, glm::vec3{0.f, 1.f, 2.f}};
		ASSERT_EQ((glm::vec3{0.f, 1.f, 2.f}), vertex.Normal());
	}
}
