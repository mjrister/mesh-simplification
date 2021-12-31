#include "geometry/half_edge.h"

#include <format>

#include <gtest/gtest.h>

using namespace geometry;
using namespace glm;
using namespace std;

namespace {
	shared_ptr<HalfEdge> MakeHalfEdge() {
		const auto v0 = make_shared<Vertex>(0, vec3{1.f});
		const auto v1 = make_shared<Vertex>(1, vec3{2.f, 0.f, 0.f});
		auto edge01 = make_shared<HalfEdge>(v1);
		const auto edge10 = make_shared<HalfEdge>(v0);
		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);
		return edge01;
	}

	TEST(HalfEdgeTest, TestFormatHalfEdge) {
		const auto edge01 = MakeHalfEdge();
		ASSERT_EQ("(0,1)", format("{}", *edge01));
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
