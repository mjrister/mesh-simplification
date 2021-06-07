#include <gtest/gtest.h>

#include "geometry/vertex.h"

using namespace geometry;

namespace {
	TEST(VertexTest, TestInsertionOperator) {
		std::ostringstream oss;
		oss << Vertex{42, glm::vec3{}, glm::vec3{}};
		ASSERT_EQ("42", oss.str());
	}

	TEST(VertexTest, TestEqualVerticesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		ASSERT_EQ(hash_value(v0), hash_value(Vertex{v0}));
	}

	TEST(VertexTest, TestEqualVertexPairsProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		ASSERT_EQ(hash_value(v0, v1), hash_value(Vertex{v0}, Vertex{v1}));
	}

	TEST(VertexTest, TestEqualVertexTriplesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		const Vertex v2{2, glm::vec3{4.f}, glm::vec3{5.f}};
		ASSERT_EQ(hash_value(v0, v1, v2), hash_value(Vertex{v0}, Vertex{v1}, Vertex{v2}));
	}
}
