#include "geometry/face.cpp"

#include <array>

#include <gtest/gtest.h>

namespace {

std::array<std::shared_ptr<gfx::Vertex>, 3> CreateValidTriangle() {
  const auto v0 = std::make_shared<gfx::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<gfx::Vertex>(1, glm::vec3{1.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<gfx::Vertex>(2, glm::vec3{0.0f, 0.5f, 0.0f});
  return std::array{v0, v1, v2};
}

TEST(FaceTest, TestFaceInitializationVertexOrder) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  const gfx::Face face120{v1, v2, v0};
  const gfx::Face face201{v2, v0, v1};

  EXPECT_EQ(face012.v0(), v0);
  EXPECT_EQ(face012.v1(), v1);
  EXPECT_EQ(face012.v2(), v2);

  EXPECT_EQ(face120.v0(), v0);
  EXPECT_EQ(face120.v1(), v1);
  EXPECT_EQ(face120.v2(), v2);

  EXPECT_EQ(face201.v0(), v0);
  EXPECT_EQ(face201.v1(), v1);
  EXPECT_EQ(face201.v2(), v2);
}

TEST(FaceTest, TestGetArea) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  EXPECT_FLOAT_EQ(1.5f, face012.area());
}

TEST(FaceTest, TestGetNormal) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  EXPECT_EQ((glm::vec3{0.0f, 0.0f, 1.0f}), face012.normal());
}

TEST(FaceTest, TestEqualFacesHaveTheSameHashValue) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  const auto face012_copy = face012;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(face012, face012_copy);
  EXPECT_EQ(hash_value(face012), hash_value(face012_copy));
}

TEST(FaceTest, TestEqualFaceVerticesHaveTheSameHashValue) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  EXPECT_EQ(hash_value(face012), hash_value(*v0, *v1, *v2));
}

#ifndef NDEBUG

TEST(FaceTest, TestGetExpiredVertexCausesProgramExit) {
  std::unique_ptr<gfx::Face> face012;
  {
    const auto [v0, v1, v2] = CreateValidTriangle();
    face012 = std::make_unique<gfx::Face>(v0, v1, v2);
  }
  EXPECT_DEATH({ std::ignore = face012->v0(); }, "");
  EXPECT_DEATH({ std::ignore = face012->v1(); }, "");
  EXPECT_DEATH({ std::ignore = face012->v2(); }, "");
}

TEST(FaceTest, TestFaceInitializationWithCollinearVerticesCausesProgramExit) {
  const auto v0 = std::make_shared<gfx::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<gfx::Vertex>(1, glm::vec3{0.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<gfx::Vertex>(2, glm::vec3{1.0f, -1.0f, 0.0f});
  EXPECT_DEATH((gfx::Face{v0, v1, v2}), "");
}

#endif

}  // namespace
