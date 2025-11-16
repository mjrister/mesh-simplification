#include "geometry/vertex.h"

#include <filesystem>
#include <memory>

#include <gtest/gtest.h>

#include "geometry/half_edge.h"

namespace {

using namespace gfx;  // NOLINT

TEST(VertexTest, TestGetVertexId) {
  static constexpr auto kId = 1;
  const Vertex vertex{kId, glm::vec3{0.0}};
  EXPECT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestSetVertexId) {
  static constexpr auto kId = 1;
  Vertex vertex{glm::vec3{0.0f}};
  vertex.set_id(kId);
  EXPECT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestGetVertexPosition) {
  static constexpr glm::vec3 kPosition{1.0f, 2.0f, 3.0f};
  const Vertex vertex{kPosition};
  EXPECT_EQ(vertex.position(), kPosition);
}

TEST(VertexTest, TestGetVertexEdge) {
  const auto vertex = std::make_shared<Vertex>(0, glm::vec3{0.0f});
  const auto edge = std::make_shared<HalfEdge>(vertex);
  vertex->set_edge(edge);
  EXPECT_EQ(vertex->edge(), edge);
}

TEST(VertexTest, TestEqualVerticesHaveTheSameHashValue) {
  const Vertex vertex{0, glm::vec3{0.0f}};
  const auto vertex_copy = vertex;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(vertex, vertex_copy);
  EXPECT_EQ(hash_value(vertex), hash_value(vertex_copy));
}

TEST(VertexTest, TestEqualVertexPairsHaveTheSameHashValue) {
  const Vertex v0{0, glm::vec3{0.0f}};
  const Vertex v1{1, glm::vec3{1.0f}};
  // NOLINTBEGIN(performance-unnecessary-copy-initialization)
  const auto v0_copy = v0;
  const auto v1_copy = v1;
  // NOLINTEND(performance-unnecessary-copy-initialization)
  EXPECT_EQ(hash_value(v0, v1), hash_value(v0_copy, v1_copy));
}

TEST(VertexTest, TestFlipVertexPairsDoNotHaveTheSameHashValue) {
  const Vertex v0{0, glm::vec3{0.0f}};
  const Vertex v1{1, glm::vec3{1.0f}};
  EXPECT_NE(hash_value(v0, v1), hash_value(v1, v0));
}

TEST(VertexTest, TestEqualVertexTriplesHaveTheSameHashValue) {
  const Vertex v0{0, glm::vec3{0.0f}};
  const Vertex v1{1, glm::vec3{1.0f}};
  const Vertex v2{2, glm::vec3{2.0f}};
  // NOLINTBEGIN(performance-unnecessary-copy-initialization)
  const auto v0_copy = v0;
  const auto v1_copy = v1;
  const auto v2_copy = v2;
  // NOLINTEND(performance-unnecessary-copy-initialization)
  EXPECT_EQ(hash_value(v0, v1, v2), hash_value(v0_copy, v1_copy, v2_copy));
}

#ifndef NDEBUG

TEST(VertexTest, TestGetUnsetIdCausesProgramExit) {
  const Vertex vertex{glm::vec3{0.0}};
  EXPECT_DEATH({ std::ignore = vertex.id(); }, "");  // NOLINT(whitespace/newline)
}

TEST(VertexTest, TestGetExpiredEdgeCausesProgramExit) {
  const auto vertex = std::make_shared<Vertex>(0, glm::vec3{0.0f});
  {
    const auto edge = std::make_shared<HalfEdge>(vertex);
    vertex->set_edge(edge);
  }
  EXPECT_DEATH({ std::ignore = vertex->edge(); }, "");  // NOLINT(whitespace/newline)
}

TEST(VertexTest, TestSetInvalidEdgeCausesProgramExit) {
  const auto v0 = std::make_shared<Vertex>(0, glm::vec3{0.0f});
  const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.0f});
  const auto edge01 = std::make_shared<HalfEdge>(v1);
  EXPECT_DEATH(v0->set_edge(edge01), "");
}

#endif

}  // namespace
