#include <sstream>

#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> MakeHalfEdge(const std::uint64_t v0_id, const std::uint64_t v1_id) {

		const auto v0 = std::make_shared<Vertex>(v0_id, glm::vec4{}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(v1_id, glm::vec4{}, glm::vec3{});

		const auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);

		return edge01;
	}

	TEST(HalfEdgeTest, TestEqualEdgesHaveTheSameId) {
		const auto edge01 = MakeHalfEdge(0, 1);
		const auto edge01_copy{edge01};
		ASSERT_EQ(edge01->Id(), edge01_copy->Id());
	}

	TEST(HalfEdgeTest, TestEdgeHasDifferentIdThanFlipEdge) {
		const auto edge01 = MakeHalfEdge(0, 1);
		ASSERT_NE(edge01->Id(), edge01->Flip()->Id());
	}

	TEST(HalfEdgeTest, TestInsertionOperator) {
		const auto edge01 = MakeHalfEdge(0, 1);
		std::ostringstream oss;
		oss << *edge01;
		ASSERT_EQ(oss.str(), "(0,1)");
	}
}