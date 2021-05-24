#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include <glm/gtc/epsilon.hpp>

#include "engine/arcball.cpp"

namespace {

	TEST(ArcballTest, TestGetNormalizedCursorPosition) {
		constexpr auto width = 300, height = 200;

		auto cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{0.f, 0.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, 1.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{0.f, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, -1.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{width, height}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, -1.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{width, 0.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, 1.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{width/2.f, height/2.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, 0.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, 0.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{-1.f, -1.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, 1.f);

		cursor_position_ndc = GetNormalizedCursorPosition(glm::dvec2{width + 1.f, height + 1.f}, width, height);
		ASSERT_FLOAT_EQ(cursor_position_ndc.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc.y, -1.f);
	}

	TEST(ArcballTest, TestGetArcballPositionForCursorInsideSphere) {
		constexpr auto x = 0.5f, y = .25f;
		constexpr glm::vec2 cursor_position_ndc{x, y};
		const auto arcball_position = GetArcballPosition(cursor_position_ndc);
		ASSERT_FLOAT_EQ(arcball_position.x, x);
		ASSERT_FLOAT_EQ(arcball_position.y, y);
		ASSERT_FLOAT_EQ(arcball_position.z, .82915622f);
	}

	TEST(ArcballTest, TestGetArcballPositionForCursorOutsideSphere) {
		const glm::vec2 cursor_position_ndc{.75f, .85f};
		const auto arcball_position = GetArcballPosition(cursor_position_ndc);
		ASSERT_FLOAT_EQ(arcball_position.x, .66162163f);
		ASSERT_FLOAT_EQ(arcball_position.y, .74983788f);
		ASSERT_FLOAT_EQ(arcball_position.z, 0.f);
	}
}