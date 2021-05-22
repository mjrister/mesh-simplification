#include <gtest/gtest.h>

#include "engine/arcball.cpp"

namespace {

	TEST(ArcballTest, TestGetNormalizedCursorPosition) {
		constexpr auto width{300}, height{200};

		auto cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{0.f, 0.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{0.f, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width, 0.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width/2.f, height/2.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 0.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 0.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{-1.f, -1.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, 1.f);

		cursor_position_norm = GetNormalizedCursorPosition(glm::dvec2{width + 1.f, height + 1.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_norm.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_norm.y, -1.f);
	}
}