#include <gtest/gtest.h>

#include "engine/arcball.cpp"

namespace {

	TEST(ArcballTest, TestGetNormalizedCursorPosition) {
		constexpr auto width{300}, height{200};

		auto cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{0.0, 0.0}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{0.0, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width, 0.0}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width/2.0, height/2.0}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 0.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 0.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{-1.0f, -1.0f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.0f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width + 1.0, height + 1.0}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.0f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.0f);
	}
}