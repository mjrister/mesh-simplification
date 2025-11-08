#include "graphics/arcball.cpp"

#include <gtest/gtest.h>

namespace {

TEST(ArcballTest, TestGetNormalizedDeviceCoordinates) {
  static constexpr auto width = 300;
  static constexpr auto height = 200;
  static constexpr auto window_dimensions = std::make_pair(width, height);

  constexpr auto cursor_position_ndc0 = GetNormalizedDeviceCoordinates(glm::vec2{0.0f, 0.0f}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc0.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc0.y, 1.0f);

  constexpr auto cursor_position_ndc1 = GetNormalizedDeviceCoordinates(glm::vec2{0.0f, height}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc1.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc1.y, -1.0f);

  constexpr auto cursor_position_ndc2 = GetNormalizedDeviceCoordinates(glm::vec2{width, height}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc2.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc2.y, -1.0f);

  constexpr auto cursor_position_ndc3 = GetNormalizedDeviceCoordinates(glm::vec2{width, 0.0f}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc3.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc3.y, 1.0f);

  constexpr auto cursor_position_ndc4 =
      GetNormalizedDeviceCoordinates(glm::vec2{width / 2.0f, height / 2.0f}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc4.x, 0.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc4.y, 0.0f);

  constexpr auto cursor_position_ndc5 = GetNormalizedDeviceCoordinates(glm::vec2{-1.0f, -1.0f}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc5.x, -1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc5.y, 1.0f);

  constexpr auto cursor_position_ndc6 =
      GetNormalizedDeviceCoordinates(glm::vec2{width + 1.0f, height + 1.0f}, window_dimensions);
  EXPECT_FLOAT_EQ(cursor_position_ndc6.x, 1.0f);
  EXPECT_FLOAT_EQ(cursor_position_ndc6.y, -1.0f);
}

TEST(ArcballTest, TestGetArcballPositionForCursorInsideUnitSphere) {
  constexpr auto x = 0.5f, y = 0.25f;
  constexpr glm::vec2 cursor_position_ndc{x, y};
  const auto arcball_position = GetArcballPosition(cursor_position_ndc);
  EXPECT_FLOAT_EQ(arcball_position.x, x);
  EXPECT_FLOAT_EQ(arcball_position.y, y);
  EXPECT_FLOAT_EQ(arcball_position.z, 0.82915622f);
}

TEST(ArcballTest, TestGetArcballPositionForCursorOutsideUnitSphere) {
  constexpr auto x = 0.75f, y = 0.85f;
  constexpr glm::vec2 cursor_position_ndc{x, y};
  const auto arcball_position = GetArcballPosition(cursor_position_ndc);
  EXPECT_FLOAT_EQ(arcball_position.x, 0.66162163f);
  EXPECT_FLOAT_EQ(arcball_position.y, 0.74983788f);
  EXPECT_FLOAT_EQ(arcball_position.z, 0.0f);
}
}  // namespace
