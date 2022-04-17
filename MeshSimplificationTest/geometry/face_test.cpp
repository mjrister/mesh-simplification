#include "geometry/face.cpp"

#include <array>
#include <format>

#include <gtest/gtest.h>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

array<shared_ptr<Vertex>, 3> MakeTriangle() {
	const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f});
	const auto v1 = make_shared<Vertex>(1, vec3{1.f, -1.f, 0.f});
	const auto v2 = make_shared<Vertex>(2, vec3{0.f, .5f, 0.f});
	return {v0, v1, v2};
}

TEST(FaceTest, TestFaceInitializationVertexOrder) {

	const auto [v0, v1, v2] = MakeTriangle();
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
	const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f});
	const auto v1 = make_shared<Vertex>(1, vec3{0.f, -1.f, 0.f});
	const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f});
	ASSERT_THROW((Face{v0, v1, v2}), invalid_argument);
}

TEST(FaceTest, TestGetFaceArea) {
	const auto [v0, v1, v2] = MakeTriangle();
	const Face face012{v0, v1, v2};
	ASSERT_FLOAT_EQ(1.5f, face012.Area());
}

TEST(FaceTEst, TestGetFaceNormal) {
	const auto [v0, v1, v2] = MakeTriangle();
	const Face face012{v0, v1, v2};
	ASSERT_EQ((vec3{0.f, 0.f, 1.f}), face012.Normal());
}

TEST(FaceTest, TestFormatFace) {
	const auto [v0, v1, v2] = MakeTriangle();
	ASSERT_EQ("(0,1,2)", format("{}", Face{v0, v1, v2}));
}

TEST(FaceTest, TestEqualFacesProduceTheSameHashValue) {
	const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f});
	const auto v1 = make_shared<Vertex>(1, vec3{0.f, .5f, 0.f});
	const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f});
	const Face face012{v0, v1, v2};
	ASSERT_EQ(hash_value(face012), hash_value(Face{face012}));
}

TEST(FaceTest, TestThreeVerticesProduceSameHashValueAsFace) {
	const auto v0 = make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f});
	const auto v1 = make_shared<Vertex>(1, vec3{0.f, .5f, 0.f});
	const auto v2 = make_shared<Vertex>(2, vec3{1.f, -1.f, 0.f});
	const Face face012{v0, v1, v2};
	ASSERT_EQ(hash_value(*v0, *v1, *v2), hash_value(face012));
}
}
