#include <sstream>

#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

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

	TEST(HalfEdgeTest, TestInsertionOperator) {
		std::ostringstream oss;
		oss << *MakeHalfEdge();
		ASSERT_EQ(oss.str(), "(0,1)");
	}

	TEST(HalfEdgeTest, TestEqualHalfEdgesProduceTheSameHashValue) {
		const auto edge01 = MakeHalfEdge();
		ASSERT_EQ(hash_value(*edge01), hash_value(HalfEdge{*edge01}));
		ASSERT_NE(hash_value(*edge01), hash_value(*edge01->Flip()));
	}

	TEST(HalfEdgeTest, TestTwoVerticesProduceSameHashValueAsHalfEdge) {
		const auto edge01 = MakeHalfEdge();
		const auto v0 = edge01->Flip()->Vertex();
		const auto v1 = edge01->Vertex();
		ASSERT_EQ(hash_value(*v0, *v1), hash_value(*edge01));
	}
}
