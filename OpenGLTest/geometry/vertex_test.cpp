#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "geometry/half_edge.h"
#include "geometry/vertex.h"

namespace {

	TEST(VertexTest, TestGetId) {
		const geometry::Vertex vertex{42, glm::vec4{}, glm::vec3{}};
		ASSERT_EQ(42, vertex.Id());
	}

	TEST(VertexTest, TestGetPosition) {
		const geometry::Vertex vertex{0, glm::vec4{42.f}, glm::vec3{}};
		ASSERT_EQ((glm::vec4{42.f}), vertex.Position());
	}

	TEST(VertexTest, TestGetNormal) {
		const geometry::Vertex vertex{0, glm::vec4{}, glm::vec3{42.f}};
		ASSERT_EQ(glm::vec3{42.f}, vertex.Normal());
	}

	TEST(VertexTest, TestGetEdge) {
		const auto vertex = std::make_shared<geometry::Vertex>(42, glm::vec4{}, glm::vec3{});
		const auto edge = std::make_shared<geometry::HalfEdge>(vertex);
		vertex->SetEdge(edge);
		ASSERT_EQ(edge, vertex->Edge());
	}
}