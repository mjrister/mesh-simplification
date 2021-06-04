#include <sstream>

#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> MakeHalfEdge(const std::uint64_t v0_id, const std::uint64_t v1_id) {
		const auto v0 = std::make_shared<Vertex>(v0_id, glm::vec3{}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(v1_id, glm::vec3{}, glm::vec3{});
		auto edge01 = std::make_shared<HalfEdge>(v1);
		const auto edge10 = std::make_shared<HalfEdge>(v0);
		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);
		return edge01;
	}

	TEST(HalfEdgeTest, TestInsertionOperator) {
		std::ostringstream oss;
		oss << *MakeHalfEdge(0, 1);
		ASSERT_EQ(oss.str(), "(0,1)");
	}
}
