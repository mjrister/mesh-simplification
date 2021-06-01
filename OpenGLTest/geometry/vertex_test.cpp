#include <gtest/gtest.h>

#include "geometry/vertex.h"

using namespace geometry;

namespace {
	TEST(VertexTest, TestAverageVertices) {
		const Vertex v0{0, glm::vec3{1.f}, glm::vec3{2.f}};
		const Vertex v1{1, glm::vec3{3.f}, glm::vec3{4.f}};
		ASSERT_EQ((Vertex{2, glm::vec3{2.f}, glm::vec3{3.f}}), Vertex::Average(2, v0, v1));
	}

	TEST(VertexTest, TestVertexEquality) {
		const Vertex v0{0, {0.f, 1.f, 2.f}, {3.f, 4.f, 5.f}};
		ASSERT_TRUE(v0 == v0);
		ASSERT_FALSE((Vertex{0, {}, {}} == Vertex{1, {}, {} }));
		ASSERT_FALSE((Vertex{0, glm::vec3{0.f}, {}} == Vertex{0, glm::vec3{1.f}, {}}));
		ASSERT_FALSE((Vertex{0, {}, glm::vec3{0.f}} == Vertex{0, {}, glm::vec3{1.f}}));
	}

	TEST(VertexTest, TestInsertionOperator) {
		std::ostringstream oss;
		oss << Vertex{42, glm::vec3{}, glm::vec3{}};
		ASSERT_EQ("42", oss.str());
	}
}
