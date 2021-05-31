#include <array>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

#include "geometry/face.h"
#include "geometry/vertex.h"

using namespace geometry;

namespace {
	TEST(FaceTest, TestFaceInitializationVertexOrder) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{0.f, .5f, 0.f, 1.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});

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

	TEST(FaceTest, TestEquivalentFacesHaveSameId) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{0.f, .5f, 0.f, 1.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});

		const Face face012{v0, v1, v2};
		const Face face120{v1, v2, v0};
		const Face face201{v2, v0, v1};

		ASSERT_EQ(face012.Id(), face120.Id());
		ASSERT_EQ(face120.Id(), face201.Id());
	}

	TEST(FaceTest, TestFaceInitializationWithCollinearVerticesThrowsException) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{0.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});

		ASSERT_THROW((Face{v0, v1, v2}), std::invalid_argument);
	}

	TEST(FaceTest, TestFaceEquality) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{0.f, .5f, 0.f, 1.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});

		const Face face012{v0, v1, v2};
		const Face face120{v1, v2, v0};
		const Face face201{v2, v0, v1};

		ASSERT_EQ(face012, face012);
		ASSERT_EQ(face012, face120);
		ASSERT_EQ(face012, face201);
	}

	TEST(FaceTest, TestFaceStreamOperator) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec4{-1.f, -1.f, 0.f, 1.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec4{0.f, .5f, 0.f, 1.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec4{1.f, -1.f, 0.f, 1.f}, glm::vec3{});

		std::ostringstream oss;
		oss << Face{v0, v1, v2};

		ASSERT_EQ("(0,1,2)", oss.str());
	}
}
