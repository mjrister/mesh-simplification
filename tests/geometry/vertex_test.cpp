#include "geometry/vertex.h"

#include <gtest/gtest.h>

#include <filesystem>

#include "geometry/half_edge.h"

namespace {

TEST(VertexTest, TestGetVertexId) {
  constexpr auto kId = 7;
  const gfx::Vertex vertex{kId, glm::vec3{0.0}};
  EXPECT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestSetVertexId) {
  constexpr auto kId = 7;
  gfx::Vertex vertex{glm::vec3{0.0f}};
  vertex.set_id(kId);
  EXPECT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestGetVertexPosition) {
  constexpr glm::vec3 kPosition{1.0f, 2.0f, 3.0f};
  const gfx::Vertex vertex{kPosition};
  EXPECT_EQ(vertex.position(), kPosition);
}

TEST(VertexTest, TestGetVertexEdge) {
  const auto vertex = std::make_shared<gfx::Vertex>(7, glm::vec3{0.0f});
  const auto edge = std::make_shared<gfx::HalfEdge>(vertex);
  vertex->set_edge(edge);
  EXPECT_EQ(vertex->edge(), edge);
}

TEST(VertexTest, TestEqualVerticesHaveTheSameHashValue) {
  const gfx::Vertex vertex{0, glm::vec3{0.0f}};
  const auto vertex_copy = vertex;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(vertex, vertex_copy);
  EXPECT_EQ(hash_value(vertex), hash_value(vertex_copy));
}

TEST(VertexTest, TestEqualVertexPairsHaveTheSameHashValue) {
  const gfx::Vertex v0{0, glm::vec3{0.0f}};
  const gfx::Vertex v1{1, glm::vec3{0.0f}};
  EXPECT_EQ(hash_value(v0, v1), hash_value(gfx::Vertex{v0}, gfx::Vertex{v1}));
}

TEST(VertexTest, TestFlipVertexPairsDoNotHaveTheSameHashValue) {
  const gfx::Vertex v0{0, glm::vec3{0.0f}};
  const gfx::Vertex v1{1, glm::vec3{0.0f}};
  EXPECT_NE(hash_value(v0, v1), hash_value(v1, v0));
}

TEST(VertexTest, TestEqualVertexTriplesHaveTheSameHashValue) {
  const gfx::Vertex v0{0, glm::vec3{0.0f}};
  const gfx::Vertex v1{1, glm::vec3{0.0f}};
  const gfx::Vertex v2{2, glm::vec3{0.0f}};
  EXPECT_EQ(hash_value(v0, v1, v2), hash_value(gfx::Vertex{v0}, gfx::Vertex{v1}, gfx::Vertex{v2}));
}

#ifndef NDEBUG

TEST(VertexTest, TestGetUnsetIdCausesProgramExit) {
  const gfx::Vertex vertex{glm::vec3{}};
  EXPECT_DEATH({ std::ignore = vertex.id(); }, "");
}

TEST(VertexTest, TestGetExpiredEdgeCausesProgramExit) {
  const auto vertex = std::make_shared<gfx::Vertex>(0, glm::vec3{0.0f});
  {
    const auto edge = std::make_shared<gfx::HalfEdge>(vertex);
    vertex->set_edge(edge);
  }
  EXPECT_DEATH({ std::ignore = vertex->edge(); }, "");
}

TEST(VertexTest, TestSetInvalidEdgeCausesProgramExit) {
  const auto v0 = std::make_shared<gfx::Vertex>(0, glm::vec3{0.0f});
  const auto v1 = std::make_shared<gfx::Vertex>(1, glm::vec3{0.0f});
  const auto edge01 = std::make_shared<gfx::HalfEdge>(v1);
  EXPECT_DEATH(v0->set_edge(edge01), "");
}

#endif

}  // namespace
