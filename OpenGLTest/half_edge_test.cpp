#include <sstream>

#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> MakeHalfEdge() {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{}, glm::vec3{});

		const auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);

		return edge01;
	}

	TEST(HalfEdgeTest, TestHalfEdgeStreamOperator) {
		std::ostringstream oss;
		oss << *MakeHalfEdge();
		ASSERT_EQ(oss.str(), "(0,1)");
	}
}