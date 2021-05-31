#include <array>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "geometry/face.cpp"
#include "geometry/vertex.h"

namespace {
	TEST(FaceTest, TestGetMinVertexOrder) {
		const auto v0 = std::make_shared<geometry::Vertex>(0, glm::vec4{}, glm::vec3{});
		const auto v1 = std::make_shared<geometry::Vertex>(1, glm::vec4{}, glm::vec3{});
		const auto v2 = std::make_shared<geometry::Vertex>(2, glm::vec4{}, glm::vec3{});
		const std::array<std::shared_ptr<geometry::Vertex>, 3> min_vertex_order{v0, v1, v2};
		ASSERT_EQ(min_vertex_order, GetMinVertexOrder(v0, v1, v2));
		ASSERT_EQ(min_vertex_order, GetMinVertexOrder(v1, v2, v0));
		ASSERT_EQ(min_vertex_order, GetMinVertexOrder(v2, v0, v1));
	}

	TEST(FaceTest, TestIsTriangle) {
		auto v0 = std::make_shared<geometry::Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		auto v1 = std::make_shared<geometry::Vertex>(1, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		auto v2 = std::make_shared<geometry::Vertex>(2, glm::vec4{0.f, 1.f, 0.f, 1.f}, glm::vec3{});
		ASSERT_TRUE(IsTriangle(v0, v1, v2));

		v0 = std::make_shared<geometry::Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		v1 = std::make_shared<geometry::Vertex>(0, glm::vec4{0.f, -1.f, 0.f, 1.f}, glm::vec3{});
		v2 = std::make_shared<geometry::Vertex>(0, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		ASSERT_FALSE(IsTriangle(v0, v1, v2));
	}

	TEST(FaceTest, TestGetFaceId) {
		const auto v0 = std::make_shared<geometry::Vertex>(0, glm::vec4{}, glm::vec3{});
		const auto v1 = std::make_shared<geometry::Vertex>(1, glm::vec4{}, glm::vec3{});
		const auto v2 = std::make_shared<geometry::Vertex>(2, glm::vec4{}, glm::vec3{});
	}
}