#include "graphics/arcball.cpp"  // NOLINT

#include <gtest/gtest.h>

namespace {

using namespace gfx::arcball;  // NOLINT

TEST(ArcballTest, TestGetNormalizedDeviceCoordinates) {
  static constexpr auto kWidth = 300;
  static constexpr auto kHeight = 200;
  static constexpr auto kWindowDimensions = std::make_pair(kWidth, kHeight);

  const auto cursor_position_ndc0 = GetNormalizedDeviceCoordinates(glm::vec2{0.0f, 0.0f}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc0.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc0.y, 1.0f);

  const auto cursor_position_ndc1 = GetNormalizedDeviceCoordinates(glm::vec2{0.0f, kHeight}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc1.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc1.y, -1.0f);

  const auto cursor_position_ndc2 = GetNormalizedDeviceCoordinates(glm::vec2{kWidth, kHeight}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc2.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc2.y, -1.0f);

  const auto cursor_position_ndc3 = GetNormalizedDeviceCoordinates(glm::vec2{kWidth, 0.0f}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc3.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc3.y, 1.0f);

  const auto cursor_position_ndc4 =
      GetNormalizedDeviceCoordinates(glm::vec2{kWidth / 2.0f, kHeight / 2.0f}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc4.x, 0.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc4.y, 0.0f);

  const auto cursor_position_ndc5 = GetNormalizedDeviceCoordinates(glm::vec2{-1.0f, -1.0f}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc5.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc5.y, 1.0f);

  const auto cursor_position_ndc6 =
      GetNormalizedDeviceCoordinates(glm::vec2{kWidth + 1.0f, kHeight + 1.0f}, kWindowDimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc6.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc6.y, -1.0f);
}

TEST(ArcballTest, TestGetArcballPositionForCursorInsideUnitSphere) {
  static constexpr auto kX = 0.5f, kY = 0.25f;
  static constexpr glm::vec2 kNormalizedCursorPosition{kX, kY};
  const auto arcball_position = GetArcballPosition(kNormalizedCursorPosition);
  EXPECT_FLOAT_EQ(arcball_position.x, kX);
  EXPECT_FLOAT_EQ(arcball_position.y, kY);
  EXPECT_FLOAT_EQ(arcball_position.z, 0.82915622f);
}

TEST(ArcballTest, TestGetArcballPositionForCursorOutsideUnitSphere) {
  static constexpr auto kX = 0.75f, kY = 0.85f;
  static constexpr glm::vec2 kNormalizedCursorPosition{kX, kY};
  const auto arcball_position = GetArcballPosition(kNormalizedCursorPosition);
  EXPECT_FLOAT_EQ(arcball_position.x, 0.66162163f);
  EXPECT_FLOAT_EQ(arcball_position.y, 0.74983788f);
  EXPECT_FLOAT_EQ(arcball_position.z, 0.0f);
}

}  // namespace
