#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

using namespace geometry;

namespace {

	TEST(VertexTest, TestGetId) {
		const Vertex vertex{42, glm::vec4{}, glm::vec3{}};
		ASSERT_EQ(42, vertex.Id());
	}

	TEST(VertexTest, TestGetPosition) {
		const Vertex vertex{0, glm::vec4{42.f}, glm::vec3{}};
		ASSERT_EQ((glm::vec4{42.f}), vertex.Position());
	}

	TEST(VertexTest, TestGetNormal) {
		const Vertex vertex{0, glm::vec4{}, glm::vec3{42.f}};
		ASSERT_EQ(glm::vec3{42.f}, vertex.Normal());
	}

	TEST(VertexTest, TestGetEdge) {
		const auto vertex = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const auto edge = std::make_shared<HalfEdge>(vertex);
		vertex->SetEdge(edge);
		ASSERT_EQ(edge, vertex->Edge());
	}
}