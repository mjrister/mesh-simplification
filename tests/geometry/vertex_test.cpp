#include "geometry/vertex.h"

#include <gtest/gtest.h>

#include "geometry/half_edge.h"

namespace {

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
