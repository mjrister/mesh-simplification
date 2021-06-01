#include <gtest/gtest.h>

#include "geometry/hash.h"

using namespace geometry;

namespace {
	TEST(HashTest, TestEqualVerticesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		ASSERT_EQ(hash_value(v0), hash_value(Vertex{v0}));
	}

	TEST(HashTest, TestEqualVertexPairsProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		ASSERT_EQ(hash_value(v0, v1), hash_value(Vertex{v0}, Vertex{v1}));
	}

	TEST(HashTest, TestEqualVertexTriplesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		const Vertex v2{2, glm::vec3{4.f}, glm::vec3{5.f}};
		ASSERT_EQ(hash_value(v0, v1, v2), hash_value(Vertex{v0}, Vertex{v1}, Vertex{v2}));
	}

	TEST(HashTest, TestEqualHalfEdgesProduceTheSameHashValue) {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{0.f}, glm::vec3{1.f});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{2.f}, glm::vec3{3.f});
		const auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);
		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);
		ASSERT_EQ(hash_value(*edge01), hash_value(HalfEdge{*edge01}));
		ASSERT_NE(hash_value(*edge01), hash_value(*edge10));
	}

	TEST(HashTest, TestEqualFacesProduceTheSameHashValue) {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, .5f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		const Face face012{v0, v1, v2};
		ASSERT_EQ(hash_value(face012), hash_value(Face{face012}));
	}
}