#include "geometry/vertex.h"

#include <gtest/gtest.h>

#include "geometry/half_edge.h"

namespace {

TEST(VertexTest, TestGetVertexId) {
  constexpr auto kId = 7;
  const qem::Vertex vertex{kId, glm::vec3{0.0}};
  ASSERT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestSetVertexId) {
  constexpr auto kId = 7;
  qem::Vertex vertex{glm::vec3{0.0f}};
  vertex.set_id(kId);
  ASSERT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestGetVertexPosition) {
  constexpr glm::vec3 kPosition{1.0f, 2.0f, 3.0f};
  const qem::Vertex vertex{kPosition};
  ASSERT_EQ(vertex.position(), kPosition);
}

TEST(VertexTest, TestGetVertexEdge) {
  const auto vertex = std::make_shared<qem::Vertex>(7, glm::vec3{0.0f});
  const auto edge = std::make_shared<qem::HalfEdge>(vertex);
  vertex->set_edge(edge);
  ASSERT_EQ(vertex->edge(), edge);
}

TEST(VertexTest, TestVertexEquality) {
  constexpr auto kId = 7;
  const qem::Vertex vertex{kId, glm::vec3{0.0f}};
  ASSERT_EQ(vertex, qem::Vertex{vertex});
}

TEST(VertexTest, TestEqualVerticesHaveTheSameHashValue) {
  const qem::Vertex vertex{0, glm::vec3{0.0f}};
  ASSERT_EQ(hash_value(vertex), hash_value(qem::Vertex{vertex}));
}

TEST(VertexTest, TestEqualVertexPairsHaveTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  const qem::Vertex v1{1, glm::vec3{0.0f}};
  ASSERT_EQ(hash_value(v0, v1), hash_value(qem::Vertex{v0}, qem::Vertex{v1}));
}

TEST(VertexTest, TestOppositeVertexPairsDoNotHaveTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  const qem::Vertex v1{1, glm::vec3{0.0f}};
  ASSERT_NE(hash_value(v0, v1), hash_value(v1, v0));
}

TEST(VertexTest, TestEqualVertexTriplesHaveTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  const qem::Vertex v1{1, glm::vec3{0.0f}};
  const qem::Vertex v2{2, glm::vec3{0.0f}};
  ASSERT_EQ(hash_value(v0, v1, v2), hash_value(qem::Vertex{v0}, qem::Vertex{v1}, qem::Vertex{v2}));
}

#ifndef NDEBUG

TEST(VertexTest, TestGetExpiredEdgeCausesProgramExit) {
  const auto vertex = std::make_shared<qem::Vertex>(glm::vec3{0.0f});
  {
    const auto edge = std::make_shared<qem::HalfEdge>(vertex);
    vertex->set_edge(edge);
  }
  ASSERT_DEATH({ std::ignore = vertex->edge(); }, "");
}

#endif

}  // namespace
