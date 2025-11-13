#include "geometry/half_edge_mesh.cpp"  // NOLINT

#include <vector>

#include <GL/gl3w.h>
#include <gtest/gtest.h>

namespace {

using namespace gfx;  // NOLINT

class HalfEdgeMeshTest : public testing::Test {
protected:
  HalfEdgeMeshTest()
      : v0_{std::make_shared<Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f})},
        v1_{std::make_shared<Vertex>(1, glm::vec3{0.0f, 0.5f, 0.0f})},
        v2_{std::make_shared<Vertex>(2, glm::vec3{1.0f, -1.0f, 0.0f})},
        edge01_{std::make_shared<HalfEdge>(v1_)},
        edge10_{std::make_shared<HalfEdge>(v0_)},
        face012_{std::make_shared<Face>(v0_, v1_, v2_)},
        vertices_{{v0_->id(), v0_}, {v1_->id(), v1_}, {v2_->id(), v2_}},
        faces_{{hash_value(*face012_), face012_}} {
    edge01_->set_flip(edge10_);
    edge10_->set_flip(edge01_);
    edges_ = std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>{{hash_value(*edge01_), edge01_},
                                                                        {hash_value(*edge10_), edge10_}};
  }

  std::shared_ptr<Vertex> v0_, v1_, v2_;
  std::shared_ptr<HalfEdge> edge01_, edge10_;
  std::shared_ptr<Face> face012_;
  std::map<int, std::shared_ptr<Vertex>> vertices_;
  std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
  std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
};

static Mesh CreateValidMesh() {
  const std::vector<glm::vec3> positions{
      {1.0f, 0.0f, 0.0f},   // v0
      {2.0f, 0.0f, 0.0f},   // v1
      {0.5f, -1.0f, 0.0f},  // v2
      {1.5f, -1.0f, 0.0f},  // v3
      {2.5f, -1.0f, 0.0f},  // v4
      {3.0f, 0.0f, 0.0f},   // v5
      {2.5f, 1.0f, 0.0f},   // v6
      {1.5f, 1.0f, 0.0f},   // v7
      {0.5f, 1.0f, 0.0f},   // v8
      {0.0f, 0.0f, 0.0f}    // v9
  };

  const std::vector<GLuint> indices{
      0, 2, 3,  // f0
      0, 3, 1,  // f1
      0, 1, 7,  // f2
      0, 7, 8,  // f3
      0, 8, 9,  // f4
      0, 9, 2,  // f5
      1, 3, 4,  // f6
      1, 4, 5,  // f7
      1, 5, 6,  // f8
      1, 6, 7   // f9
  };

  return Mesh{positions, std::vector(10, glm::vec3{0.0f, 0.0f, 1.0f}), {}, indices};
}

HalfEdgeMesh MakeHalfEdgeMesh() {
  const auto mesh = CreateValidMesh();
  return HalfEdgeMesh{mesh};
}

void VerifyEdge(const std::shared_ptr<Vertex>& v0,
                const std::shared_ptr<Vertex>& v1,
                const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {
  const auto edge01_iterator = edges.find(hash_value(*v0, *v1));
  const auto edge10_iterator = edges.find(hash_value(*v1, *v0));

  ASSERT_NE(edge01_iterator, edges.end());
  ASSERT_NE(edge10_iterator, edges.end());

  const auto& edge01 = edge01_iterator->second;
  const auto& edge10 = edge10_iterator->second;

  EXPECT_EQ(v0, edge10->vertex());
  EXPECT_EQ(v1, edge01->vertex());

  EXPECT_EQ(edge01, edge10->flip());
  EXPECT_EQ(edge10, edge01->flip());

  EXPECT_EQ(edge01, edge01->flip()->flip());
  EXPECT_EQ(edge10, edge10->flip()->flip());
}

void VerifyTriangles(const HalfEdgeMesh& half_edge_mesh, const std::vector<GLuint>& indices) {
  const auto& vertices = half_edge_mesh.vertices();
  const auto& edges = half_edge_mesh.edges();
  const auto& faces = half_edge_mesh.faces();

  for (auto i = 0; std::cmp_less(i, indices.size()); i += 3) {
    const auto v0_iterator = vertices.find(indices[i]);
    const auto v1_iterator = vertices.find(indices[i + 1]);
    const auto v2_iterator = vertices.find(indices[i + 2]);

    ASSERT_NE(v0_iterator, vertices.end());
    ASSERT_NE(v1_iterator, vertices.end());
    ASSERT_NE(v2_iterator, vertices.end());

    const auto& v0 = v0_iterator->second;
    const auto& v1 = v1_iterator->second;
    const auto& v2 = v2_iterator->second;

    VerifyEdge(v0, v1, edges);
    VerifyEdge(v1, v2, edges);
    VerifyEdge(v2, v0, edges);

    const auto edge01 = edges.at(hash_value(*v0, *v1));
    const auto edge12 = edges.at(hash_value(*v1, *v2));
    const auto edge20 = edges.at(hash_value(*v2, *v0));

    EXPECT_EQ(edge01->next(), edge12);
    EXPECT_EQ(edge12->next(), edge20);
    EXPECT_EQ(edge20->next(), edge01);

    const auto face012_iterator = faces.find(hash_value(*v0, *v1, *v2));
    ASSERT_NE(face012_iterator, faces.end());

    const auto face012 = face012_iterator->second;
    EXPECT_EQ(edge01->face(), face012);
    EXPECT_EQ(edge12->face(), face012);
    EXPECT_EQ(edge20->face(), face012);
  }
}

TEST_F(HalfEdgeMeshTest, TestCreateHalfEdgeMesh) {
  const auto mesh = CreateValidMesh();
  const HalfEdgeMesh half_edge_mesh{mesh};

  EXPECT_EQ(10, half_edge_mesh.vertices().size());
  EXPECT_EQ(38, half_edge_mesh.edges().size());
  EXPECT_EQ(10, half_edge_mesh.faces().size());

  VerifyTriangles(half_edge_mesh, mesh.indices());
}

TEST_F(HalfEdgeMeshTest, TestCollapseEdge) {
  auto half_edge_mesh = MakeHalfEdgeMesh();
  const auto& vertices = half_edge_mesh.vertices();
  const auto& edges = half_edge_mesh.edges();
  const auto& v0 = vertices.at(0);
  const auto& v1 = vertices.at(1);
  const auto& edge01 = edges.at(hash_value(*v0, *v1));
  const Vertex v_new{static_cast<int>(half_edge_mesh.vertices().size()), (v0->position() + v1->position()) / 2.0f};

  half_edge_mesh.Contract(*edge01, std::make_shared<Vertex>(v_new));

  EXPECT_EQ(9, half_edge_mesh.vertices().size());
  EXPECT_EQ(32, half_edge_mesh.edges().size());
  EXPECT_EQ(8, half_edge_mesh.faces().size());

  VerifyTriangles(half_edge_mesh, {2, 3,  10,   // f0
                                   3, 4,  10,   // f1
                                   4, 5,  10,   // f2
                                   5, 6,  10,   // f3
                                   6, 7,  10,   // f4
                                   7, 8,  10,   // f5
                                   8, 9,  10,   // f6
                                   2, 10, 9});  // f7
}

TEST_F(HalfEdgeMeshTest, TestGetHalfEdge) {
  EXPECT_EQ(edge01_, GetHalfEdge(*v0_, *v1_, edges_));
  EXPECT_EQ(edge10_, GetHalfEdge(*v1_, *v0_, edges_));
}

TEST_F(HalfEdgeMeshTest, TestDeleteVertex) {
  DeleteVertex(*v0_, vertices_);
  EXPECT_EQ(vertices_.size(), 2);

  DeleteVertex(*v1_, vertices_);
  EXPECT_EQ(vertices_.size(), 1);

  DeleteVertex(*v2_, vertices_);
  EXPECT_TRUE(vertices_.empty());
}

TEST_F(HalfEdgeMeshTest, TestDeleteHalfEdge) {
  DeleteEdge(*edge01_, edges_);
  EXPECT_TRUE(edges_.empty());
}

TEST_F(HalfEdgeMeshTest, TestDeleteFace) {
  DeleteFace(*face012_, faces_);
  EXPECT_TRUE(faces_.empty());
}

#ifndef NDEBUG

TEST_F(HalfEdgeMeshTest, TestCollapseInvalidHalfEdgeCausesProgramExit) {
  auto half_edge_mesh = MakeHalfEdgeMesh();
  const auto invalid_vertex = std::make_shared<Vertex>(static_cast<int>(half_edge_mesh.vertices().size()), glm::vec3{});
  const auto invalid_half_edge = std::make_shared<HalfEdge>(invalid_vertex);
  const auto v_new = std::make_shared<Vertex>(glm::vec3{});
  invalid_half_edge->set_flip(edge01_);
  EXPECT_DEATH(half_edge_mesh.Contract(*invalid_half_edge, v_new), "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeMeshTest, TestGetInvalidHalfEdgeCausesProgramExit) {
  const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges;
  EXPECT_DEATH(GetHalfEdge(*v1_, *v0_, edges), "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeMeshTest, TestDeleteInvalidVertexCausesProgramExit) {
  std::map<int, std::shared_ptr<Vertex>> vertices;
  EXPECT_DEATH(DeleteVertex(*v0_, vertices), "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeMeshTest, TestDeleteInvalidHalfEdgeCausesProgramExit) {
  std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges;
  EXPECT_DEATH(DeleteEdge(*edge01_, edges), "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeMeshTest, TestDeleteInvalidFaceCausesProgramExit) {
  std::unordered_map<std::size_t, std::shared_ptr<Face>> faces;
  EXPECT_DEATH(DeleteFace(*face012_, faces), "");  // NOLINT(whitespace/newline)
}

#endif

}  // namespace
