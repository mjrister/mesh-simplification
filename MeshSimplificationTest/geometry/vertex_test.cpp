#include <format>

#include <gtest/gtest.h>

#include "geometry/vertex.h"

using namespace geometry;
using namespace glm;
using namespace std;

namespace {
	TEST(VertexTest, TestInsertionOperator) {
		ASSERT_EQ("42", format("{}", Vertex{42, vec3{}, vec3{}}));
	}

	TEST(VertexTest, TestEqualVerticesProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}, vec3{1.f}};
		ASSERT_EQ(hash_value(v0), hash_value(Vertex{v0}));
	}

	TEST(VertexTest, TestEqualVertexPairsProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}, vec3{1.f}};
		const Vertex v1{1, vec3{2.f}, vec3{3.f}};
		ASSERT_EQ(hash_value(v0, v1), hash_value(Vertex{v0}, Vertex{v1}));
	}

	TEST(VertexTest, TestEqualVertexTriplesProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}, vec3{1.f}};
		const Vertex v1{1, vec3{2.f}, vec3{3.f}};
		const Vertex v2{2, vec3{4.f}, vec3{5.f}};
		ASSERT_EQ(hash_value(v0, v1, v2), hash_value(Vertex{v0}, Vertex{v1}, Vertex{v2}));
	}
}
