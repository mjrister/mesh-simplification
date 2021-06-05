#include <array>

#include <gtest/gtest.h>

#include "geometry/face.cpp"
#include "geometry/vertex.h"

using namespace geometry;

namespace {
	std::array<std::shared_ptr<Vertex>, 3> MakeTriangleVertices() {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, .5f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		return {v0, v1, v2};
	}

	TEST(FaceTest, TestFaceInitializationVertexOrder) {

		const auto [v0, v1, v2] = MakeTriangleVertices();
		const Face face012{v0, v1, v2};
		const Face face120{v1, v2, v0};
		const Face face201{v2, v0, v1};

		ASSERT_EQ(face012.V0(), v0);
		ASSERT_EQ(face012.V1(), v1);
		ASSERT_EQ(face012.V2(), v2);

		ASSERT_EQ(face120.V0(), v0);
		ASSERT_EQ(face120.V1(), v1);
		ASSERT_EQ(face120.V2(), v2);

		ASSERT_EQ(face120.V0(), v0);
		ASSERT_EQ(face120.V1(), v1);
		ASSERT_EQ(face120.V2(), v2);
	}

	TEST(FaceTest, TestFaceInitializationWithCollinearVerticesThrowsException) {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, -1.f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		ASSERT_THROW((Face{v0, v1, v2}), std::invalid_argument);
	}

	TEST(FaceTest, TestInsertionOperator) {
		const auto [v0, v1, v2] = MakeTriangleVertices();
		std::ostringstream oss;
		oss << Face{v0, v1, v2};
		ASSERT_EQ("(0,1,2)", oss.str());
	}

	TEST(FaceTest, TestEqualFacesProduceTheSameHashValue) {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, .5f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		const Face face012{v0, v1, v2};
		ASSERT_EQ(hash_value(face012), hash_value(Face{face012}));
	}

	TEST(FaceTest, TestThreeVerticesProduceSameHashValueAsFace) {
		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, .5f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		const Face face012{v0, v1, v2};
		ASSERT_EQ(hash_value(*v0, *v1, *v2), hash_value(face012));
	}
}
