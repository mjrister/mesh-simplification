#include "geometry/half_edge.h"

#include <memory>

#include <gtest/gtest.h>

namespace {

using namespace gfx;  // NOLINT

class HalfEdgeTest : public testing::Test {
protected:
  HalfEdgeTest()
      :  // NOLINTBEGIN(*-magic-numbers)
        v0_{std::make_shared<Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f})},
        v1_{std::make_shared<Vertex>(1, glm::vec3{1.0f, -1.0f, 0.0f})},
        v2_{std::make_shared<Vertex>(2, glm::vec3{0.0f, 0.5f, 0.0f})},
        // NOLINTEND(*-magic-numbers)
        edge01_{std::make_shared<HalfEdge>(v1_)},
        edge10_{std::make_shared<HalfEdge>(v0_)},
        edge12_{std::make_shared<HalfEdge>(v2_)},
        edge21_{std::make_shared<HalfEdge>(v1_)},
        edge20_{std::make_shared<HalfEdge>(v0_)},
        edge02_{std::make_shared<HalfEdge>(v2_)},
        face012_{std::make_shared<Face>(v0_, v1_, v2_)} {
    edge01_->set_flip(edge10_);
    edge10_->set_flip(edge01_);
    edge12_->set_flip(edge21_);
    edge21_->set_flip(edge12_);
    edge20_->set_flip(edge02_);
    edge02_->set_flip(edge20_);

    edge01_->set_next(edge12_);
    edge12_->set_next(edge20_);
    edge20_->set_next(edge01_);

    v0_->set_edge(edge20_);
    v1_->set_edge(edge01_);
    v2_->set_edge(edge12_);

    edge01_->set_face(face012_);
    edge12_->set_face(face012_);
    edge20_->set_face(face012_);
  }

  // NOLINTBEGIN(*-private-member-variables-in-classes)
  std::shared_ptr<Vertex> v0_, v1_, v2_;
  std::shared_ptr<HalfEdge> edge01_, edge10_, edge12_, edge21_, edge20_, edge02_;
  std::shared_ptr<Face> face012_;
  // NOLINTEND(*-private-member-variables-in-classes)
};

TEST_F(HalfEdgeTest, TestGetVertex) {
  EXPECT_EQ(*v0_, *edge10_->vertex());
  EXPECT_EQ(*v1_, *edge01_->vertex());
  EXPECT_EQ(*v2_, *edge12_->vertex());
  EXPECT_EQ(*v1_, *edge21_->vertex());
  EXPECT_EQ(*v0_, *edge20_->vertex());
  EXPECT_EQ(*v2_, *edge02_->vertex());
}

TEST_F(HalfEdgeTest, TestGetFlip) {
  EXPECT_EQ(*edge01_->flip(), *edge10_);
  EXPECT_EQ(*edge10_->flip(), *edge01_);
  EXPECT_EQ(*edge12_->flip(), *edge21_);
  EXPECT_EQ(*edge21_->flip(), *edge12_);
  EXPECT_EQ(*edge20_->flip(), *edge02_);
  EXPECT_EQ(*edge02_->flip(), *edge20_);
}

TEST_F(HalfEdgeTest, TestGetNext) {
  EXPECT_EQ(*edge01_->next(), *edge12_);
  EXPECT_EQ(*edge12_->next(), *edge20_);
  EXPECT_EQ(*edge20_->next(), *edge01_);
}

TEST_F(HalfEdgeTest, TestGetFace) {
  EXPECT_EQ(edge01_->face(), face012_);
  EXPECT_EQ(edge12_->face(), face012_);
  EXPECT_EQ(edge20_->face(), face012_);
}

TEST_F(HalfEdgeTest, TestEqualHalfEdgesHaveTheSameHashValue) {
  const auto edge01_copy = *edge01_;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(*edge01_, edge01_copy);
  EXPECT_EQ(hash_value(*edge01_), hash_value(edge01_copy));
}

TEST_F(HalfEdgeTest, TestEqualHalfEdgeVerticesHaveTheSameHashValue) {
  const auto v0 = edge01_->flip()->vertex();
  const auto v1 = edge01_->vertex();
  EXPECT_EQ(hash_value(*v0, *v1), hash_value(*edge01_));
}

TEST_F(HalfEdgeTest, TestFlipHalfEdgesDoNotHaveTheSameHashValue) {
  EXPECT_NE(hash_value(*edge01_), hash_value(*edge01_->flip()));
}

#ifndef NDEBUG

TEST_F(HalfEdgeTest, TestGetExpiredVertexCausesProgramExit) {
  std::shared_ptr<HalfEdge> edge10;
  {
    const auto v0 = std::make_shared<Vertex>(0, glm::vec3{});
    edge10 = std::make_shared<HalfEdge>(v0);
  }
  EXPECT_DEATH({ std::ignore = edge10->vertex(); }, "");  // NOLINT
}

TEST_F(HalfEdgeTest, TestGetExpiredFlipEdgeCausesProgramExit) {
  {
    const auto edge10 = std::make_shared<HalfEdge>(v0_);
    edge01_->set_flip(edge10);
  }
  EXPECT_DEATH({ std::ignore = edge01_->flip(); }, "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeTest, TestGetExpiredNextEdgeCausesProgramExit) {
  {
    const auto edge12 = std::make_shared<HalfEdge>(v2_);
    edge01_->set_next(edge12);
  }
  EXPECT_DEATH({ std::ignore = edge01_->next(); }, "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeTest, TestGetExpiredFaceCausesProgramExit) {
  {
    const auto face012 = std::make_shared<Face>(v0_, v1_, v2_);
    edge01_->set_face(face012);
  }
  EXPECT_DEATH({ std::ignore = edge01_->face(); }, "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeTest, TestSetInvalidFlipCausesProgramExit) {
  EXPECT_DEATH(edge01_->set_flip(edge01_), "");          // NOLINT(whitespace/newline)
  EXPECT_DEATH(edge01_->set_flip(edge01_->next()), "");  // NOLINT(whitespace/newline)
}

TEST_F(HalfEdgeTest, TestSetInvalidNextCausesProgramExit) {
  EXPECT_DEATH(edge01_->set_next(edge01_), "");          // NOLINT(whitespace/newline)
  EXPECT_DEATH(edge01_->set_next(edge01_->flip()), "");  // NOLINT(whitespace/newline)
}

#endif

}  // namespace
