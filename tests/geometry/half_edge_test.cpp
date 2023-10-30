#include "geometry/half_edge.h"

#include <format>

#include <gtest/gtest.h>

namespace {

class HalfEdgeTest : public testing::Test {
protected:
  HalfEdgeTest()
      : v0_{std::make_shared<qem::Vertex>(0, glm::vec3{1.0f})},
        v1_{std::make_shared<qem::Vertex>(1, glm::vec3{2.0f, 0.0f, 0.0f})},
        edge01_{std::make_shared<qem::HalfEdge>(v1_)},
        edge10_{std::make_shared<qem::HalfEdge>(v0_)} {
    edge01_->set_flip(edge10_);
    edge10_->set_flip(edge01_);
  }

  std::shared_ptr<qem::Vertex> v0_, v1_;
  std::shared_ptr<qem::HalfEdge> edge01_, edge10_;
};

TEST_F(HalfEdgeTest, TestEqualHalfEdgesProduceTheSameHashValue) {
  ASSERT_EQ(hash_value(*edge01_), hash_value(qem::HalfEdge{*edge01_}));
  ASSERT_NE(hash_value(*edge01_), hash_value(*edge01_->flip()));
}

TEST_F(HalfEdgeTest, TestTwoVerticesProduceSameHashValueAsHalfEdge) {
  const auto v0 = edge01_->flip()->vertex();
  const auto v1 = edge01_->vertex();
  ASSERT_EQ(hash_value(*v0, *v1), hash_value(*edge01_));
}

}  // namespace
