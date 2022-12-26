#include "geometry/vertex.h"

#include <format>

#include <gtest/gtest.h>

namespace {

TEST(VertexTest, TestEqualVerticesProduceTheSameHashValue) {
	const qem::Vertex v0{0, glm::vec3{0.0f}};
	ASSERT_EQ(hash_value(v0), hash_value(qem::Vertex{v0}));
}

TEST(VertexTest, TestEqualVertexPairsProduceTheSameHashValue) {
	const qem::Vertex v0{0, glm::vec3{0.0f}};
	const qem::Vertex v1{1, glm::vec3{2.0f}};
	ASSERT_EQ(hash_value(v0, v1), hash_value(qem::Vertex{v0}, qem::Vertex{v1}));
}

TEST(VertexTest, TestEqualVertexTriplesProduceTheSameHashValue) {
	const qem::Vertex v0{0, glm::vec3{0.0f}};
	const qem::Vertex v1{1, glm::vec3{2.0f}};
	const qem::Vertex v2{2, glm::vec3{4.0f}};
	ASSERT_EQ(hash_value(v0, v1, v2), hash_value(qem::Vertex{v0}, qem::Vertex{v1}, qem::Vertex{v2}));
}
}
