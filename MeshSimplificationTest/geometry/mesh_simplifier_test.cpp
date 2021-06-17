#include <gtest/gtest.h>

#include "geometry/mesh_simplifier.cpp"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> MakeHalfEdge() {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{}, glm::vec3{});
		auto edge01 = std::make_shared<HalfEdge>(v1);
		const auto edge10 = std::make_shared<HalfEdge>(v0);
		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);
		return edge01;
	}

	TEST(MeshSimplifierTest, TestGetMinEdge) {
		const auto edge01 = MakeHalfEdge();
		ASSERT_EQ(edge01->Flip(), GetMinEdge(edge01));
	}

	TEST(MeshSimplifierTest, TestAverageVertices) {
		const Vertex v0{0, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{3.f}};
		const auto average = AverageVertices(2, v0, v1);
		ASSERT_EQ(2, average->Id());
		ASSERT_EQ(glm::vec3{2.f}, average->Position());
	}
}
