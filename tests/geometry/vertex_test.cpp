#include "geometry/vertex.h"

#include <gtest/gtest.h>

#include "geometry/half_edge.h"

namespace {

TEST(VertexTest, TestVertexConstructionWithPosition) {
  constexpr glm::vec3 kPosition{1.0f, 2.0f, 3.0f};
  const qem::Vertex vertex{kPosition};
  ASSERT_EQ(vertex.position(), kPosition);
}

TEST(VertexTest, TestVertexConstructionWithIdAndPosition) {
  constexpr std::uint64_t kId = 42;
  constexpr glm::vec3 kPosition{1.0f, 2.0f, 3.0f};
  const qem::Vertex vertex{kId, kPosition};
  ASSERT_EQ(vertex.id(), kId);
  ASSERT_EQ(vertex.position(), kPosition);
}

TEST(VertexTest, TestSetVertexId) {
  constexpr std::uint64_t kId = 42;
  qem::Vertex vertex{glm::vec3{}};
  vertex.set_id(kId);
  ASSERT_EQ(vertex.id(), kId);
}

TEST(VertexTest, TestEqualVerticesProduceTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  ASSERT_EQ(hash_value(v0), hash_value(qem::Vertex{v0}));
}

TEST(VertexTest, TestEqualVertexPairsProduceTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  const qem::Vertex v1{1, glm::vec3{2.0f}};
  ASSERT_EQ(hash_value(v0, v1), hash_value(qem::Vertex{v0}, qem::Vertex{v1}));
}

TEST(VertexTest, TestEqualVertexTriplesProduceTheSameHashValue) {
  const qem::Vertex v0{0, glm::vec3{0.0f}};
  const qem::Vertex v1{1, glm::vec3{2.0f}};
  const qem::Vertex v2{2, glm::vec3{4.0f}};
  ASSERT_EQ(hash_value(v0, v1, v2), hash_value(qem::Vertex{v0}, qem::Vertex{v1}, qem::Vertex{v2}));
}

#ifndef NDEBUG

TEST(VertexTest, TestGetExpiredEdgeCausesProgramExit) {
  const auto vertex = std::make_shared<qem::Vertex>(glm::vec3{});
  {
    const auto edge = std::make_shared<qem::HalfEdge>(vertex);
    vertex->set_edge(edge);
  }
  ASSERT_DEATH({ std::ignore = vertex->edge(); }, "");
}

#endif

}  // namespace
