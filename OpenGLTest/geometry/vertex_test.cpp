#include "geometry/vertex.h"

#include <gtest/gtest.h>

namespace {
	using namespace geometry;

	TEST(Vertex, TestGetVertexId) {
		const Vertex vertex{42, {}, {}};
		ASSERT_EQ(42, vertex.Id());
	}

	TEST(Vertex, TestGetVertexPosition) {
		const Vertex vertex{42, {0.f, 1.f, 2.f}, {}};
		ASSERT_EQ((glm::vec3{0.f, 1.f, 2.f}), vertex.Position());
	}

	TEST(Vertex, TestGetVertexNormal) {
		const Vertex vertex{42, {}, {0.f, 1.f, 2.f}};
		ASSERT_EQ((glm::vec3{0.f, 1.f, 2.f}), vertex.Normal());
	}

	TEST(Vertex, TestVertexEquality) {
		const Vertex v0{42, {0.f, 1.f, 2.f}, {3.f, 4.f, 5.f}};
		const Vertex v1{42, {0.f, 1.f, 2.f}, {3.f, 4.f, 5.f}};
		ASSERT_EQ(v0, v1);
	}

	TEST(Vertex, TestVertexInequality) {
		ASSERT_NE((Vertex{0, {}, {}}), (Vertex{1, {}, {}}));
		ASSERT_NE((Vertex{0, {0.f, 1.f, 2.f}, {}}), (Vertex{0, {3.f, 4.f, 5.f}, {}}));
		ASSERT_NE((Vertex{0, {}, {0.f, 1.f, 2.f}}), (Vertex{0, {}, {3.f, 4.f, 5.f}}));
	}
}
