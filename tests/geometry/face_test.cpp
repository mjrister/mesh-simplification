#include "geometry/face.cpp"

#include <array>

#include <gtest/gtest.h>

namespace {

std::array<std::shared_ptr<qem::Vertex>, 3> CreateValidTriangle() {
  const auto v0 = std::make_shared<qem::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<qem::Vertex>(1, glm::vec3{1.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<qem::Vertex>(2, glm::vec3{0.0f, 0.5f, 0.0f});
  return {v0, v1, v2};
}

TEST(FaceTest, TestFaceInitializationVertexOrder) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const qem::Face face012{v0, v1, v2};
  const qem::Face face120{v1, v2, v0};
  const qem::Face face201{v2, v0, v1};

  ASSERT_EQ(face012.v0(), v0);
  ASSERT_EQ(face012.v1(), v1);
  ASSERT_EQ(face012.v2(), v2);

  ASSERT_EQ(face120.v0(), v0);
  ASSERT_EQ(face120.v1(), v1);
  ASSERT_EQ(face120.v2(), v2);

  ASSERT_EQ(face120.v0(), v0);
  ASSERT_EQ(face120.v1(), v1);
  ASSERT_EQ(face120.v2(), v2);
}

TEST(FaceTest, TestGetFaceArea) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const qem::Face face012{v0, v1, v2};
  ASSERT_FLOAT_EQ(1.5f, face012.area());
}

TEST(FaceTest, TestGetFaceNormal) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const qem::Face face012{v0, v1, v2};
  ASSERT_EQ((glm::vec3{0.0f, 0.0f, 1.0f}), face012.normal());
}

TEST(FaceTest, TestEqualFacesProduceTheSameHashValue) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const qem::Face face012{v0, v1, v2};
  ASSERT_EQ(hash_value(face012), hash_value(qem::Face{face012}));
}

TEST(FaceTest, TestThreeVerticesProduceSameHashValueAsFace) {
  const auto [v0, v1, v2] = CreateValidTriangle();
  const qem::Face face012{v0, v1, v2};
  ASSERT_EQ(hash_value(*v0, *v1, *v2), hash_value(face012));
}

#ifndef NDEBUG

TEST(FaceTest, TestGetExpiredVertex0CausesProgramExit) {
  std::unique_ptr<qem::Face> face012;
  {
    const auto [v0, v1, v2] = CreateValidTriangle();
    face012 = std::make_unique<qem::Face>(v0, v1, v2);
  }
  ASSERT_DEATH({ std::ignore = face012->v0(); }, "");
}

TEST(FaceTest, TestGetExpiredVertex1CausesProgramExit) {
  std::unique_ptr<qem::Face> face012;
  {
    const auto [v0, v1, v2] = CreateValidTriangle();
    face012 = std::make_unique<qem::Face>(v0, v1, v2);
  }
  ASSERT_DEATH({ std::ignore = face012->v1(); }, "");
}

TEST(FaceTest, TestGetExpiredVertex2CausesProgramExit) {
  std::unique_ptr<qem::Face> face012;
  {
    const auto [v0, v1, v2] = CreateValidTriangle();
    face012 = std::make_unique<qem::Face>(v0, v1, v2);
  }
  ASSERT_DEATH({ std::ignore = face012->v2(); }, "");
}

TEST(FaceTest, TestFaceInitializationWithCollinearVerticesCausesProgramExit) {
  const auto v0 = std::make_shared<qem::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<qem::Vertex>(1, glm::vec3{0.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<qem::Vertex>(2, glm::vec3{1.0f, -1.0f, 0.0f});
  ASSERT_DEATH((qem::Face{v0, v1, v2}), "");
}

#endif

}  // namespace
