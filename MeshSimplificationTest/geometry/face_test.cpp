#include "geometry/face.cpp"

#include <array>
#include <format>

#include <gtest/gtest.h>

using namespace geometry;
using namespace glm;
using namespace std;

namespace {
	array<shared_ptr<Vertex>, 3> MakeTriangleVertices() {
		const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f}, vec3{});
		const auto v1 = make_shared<Vertex>(1, vec3{0.f, .5f, 0.f}, vec3{});
		const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f}, vec3{});
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
		const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f}, vec3{});
		const auto v1 = make_shared<Vertex>(1, vec3{0.f, -1.f, 0.f}, vec3{});
		const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f}, vec3{});
		ASSERT_THROW((Face{v0, v1, v2}), invalid_argument);
	}

	TEST(FaceTest, TestInsertionOperator) {
		const auto [v0, v1, v2] = MakeTriangleVertices();
		ASSERT_EQ("(0,1,2)", format("{}", Face{v0, v1, v2}));
	}

	TEST(FaceTest, TestEqualFacesProduceTheSameHashValue) {
		const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f}, vec3{});
		const auto v1 = make_shared<Vertex>(1, vec3{0.f, .5f, 0.f}, vec3{});
		const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f}, vec3{});
		const Face face012{v0, v1, v2};
		ASSERT_EQ(hash_value(face012), hash_value(Face{face012}));
	}

	TEST(FaceTest, TestThreeVerticesProduceSameHashValueAsFace) {
		const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f}, vec3{});
		const auto v1 = make_shared<Vertex>(1, vec3{0.f, .5f, 0.f}, vec3{});
		const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f}, vec3{});
		const Face face012{v0, v1, v2};
		ASSERT_EQ(hash_value(*v0, *v1, *v2), hash_value(face012));
	}
}
