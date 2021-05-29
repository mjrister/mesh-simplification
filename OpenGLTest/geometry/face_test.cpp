#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "geometry/face.h"
#include "geometry/half_edge.h"

using namespace geometry;

namespace {

	TEST(FaceTest, TestGetV0) {
		const auto vertex = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const Face face{vertex, nullptr, nullptr};
		ASSERT_EQ(vertex, face.V0());
	}

	TEST(FaceTest, TestGetV1) {
		const auto vertex = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const Face face{nullptr, vertex, nullptr};
		ASSERT_EQ(vertex, face.V1());
	}

	TEST(FaceTest, TestGetV2) {
		const auto vertex = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const Face face{nullptr, nullptr, vertex};
		ASSERT_EQ(vertex, face.V2());
	}

	TEST(FaceTest, TestGetEdge) {
		const auto vertex = std::make_shared<Vertex>(0, glm::vec4{}, glm::vec3{});
		const auto edge = std::make_shared<HalfEdge>(vertex);
		Face face{nullptr, nullptr, nullptr};
		face.SetEdge(edge);
		ASSERT_EQ(edge, face.Edge());
	}
}
