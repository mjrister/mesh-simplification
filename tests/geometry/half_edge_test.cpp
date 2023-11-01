#include "geometry/half_edge.h"

#include <gtest/gtest.h>

namespace {

class HalfEdgeTest : public testing::Test {
protected:
  HalfEdgeTest()
      : v0_{std::make_shared<qem::Vertex>(0, glm::vec3{0.0f})},
        v1_{std::make_shared<qem::Vertex>(1, glm::vec3{0.0f})},
        edge01_{std::make_shared<qem::HalfEdge>(v1_)},
        edge10_{std::make_shared<qem::HalfEdge>(v0_)} {
    edge01_->set_flip(edge10_);
    edge10_->set_flip(edge01_);
  }

  std::shared_ptr<qem::Vertex> v0_, v1_;
  std::shared_ptr<qem::HalfEdge> edge01_, edge10_;
};

TEST_F(HalfEdgeTest, TestHalfEdgeEquality) {
  ASSERT_EQ(edge01_, edge10_->flip());
  ASSERT_EQ(edge10_, edge01_->flip());
}

TEST_F(HalfEdgeTest, TestEqualHalfEdgesHaveTheSameHashValue) {
  EXPECT_EQ(hash_value(*edge01_), hash_value(qem::HalfEdge{*edge01_}));
}

TEST_F(HalfEdgeTest, TestOppositeHalfEdgesDoNotHaveTheSameHashValue) {
  EXPECT_NE(hash_value(*edge01_), hash_value(*edge01_->flip()));
}

TEST_F(HalfEdgeTest, TestEqualHalfEdgeVerticesHaveTheSameHashValue) {
  const auto v0 = edge01_->flip()->vertex();
  const auto v1 = edge01_->vertex();
  EXPECT_EQ(hash_value(*v0, *v1), hash_value(*edge01_));
}

#ifndef NDEBUG

TEST_F(HalfEdgeTest, TestGetExpiredVertexCausesProgramExit) {
  std::shared_ptr<qem::HalfEdge> edge;
  {
    const auto vertex = std::make_shared<qem::Vertex>(0, glm::vec3{});
    edge = std::make_shared<qem::HalfEdge>(vertex);
  }
  EXPECT_DEATH({ std::ignore = edge->vertex(); }, "");
}

TEST_F(HalfEdgeTest, TestGetExpiredFlipEdgeCausesProgramExit) {
  const auto v0 = std::make_shared<qem::Vertex>(0, glm::vec3{});
  const auto v1 = std::make_shared<qem::Vertex>(1, glm::vec3{});
  const auto edge01 = std::make_shared<qem::HalfEdge>(v1);
  {
    const auto edge10 = std::make_shared<qem::HalfEdge>(v0);
    edge01->set_flip(edge10);
  }
  EXPECT_DEATH({ std::ignore = edge01->flip(); }, "");
}

TEST_F(HalfEdgeTest, TestGetExpiredNextEdgeCausesProgramExit) {
  const auto v1 = std::make_shared<qem::Vertex>(1, glm::vec3{});
  const auto v2 = std::make_shared<qem::Vertex>(2, glm::vec3{});
  const auto edge01 = std::make_shared<qem::HalfEdge>(v1);
  {
    const auto edge12 = std::make_shared<qem::HalfEdge>(v2);
    edge01->set_next(edge12);
  }
  EXPECT_DEATH({ std::ignore = edge01->next(); }, "");
}

TEST_F(HalfEdgeTest, TestGetExpiredFaceCausesProgramExit) {
  const auto v0 = std::make_shared<qem::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<qem::Vertex>(1, glm::vec3{1.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<qem::Vertex>(2, glm::vec3{0.0f, 0.5f, 0.0f});
  const auto edge01 = std::make_shared<qem::HalfEdge>(v1);
  {
    const auto face012 = std::make_shared<qem::Face>(v0, v1, v2);
    edge01->set_face(face012);
  }
  EXPECT_DEATH({ std::ignore = edge01->face(); }, "");
}

#endif

}  // namespace
