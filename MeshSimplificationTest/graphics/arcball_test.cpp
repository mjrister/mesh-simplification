#include <gtest/gtest.h>

#include "graphics/arcball.cpp"

using namespace gfx;
using namespace glm;
using namespace std;

namespace {
	TEST(ArcballTest, TestGetNormalizedDeviceCoordinates) {
		constexpr auto width = 300;
		constexpr auto height = 200;
		constexpr auto window_dimensions = make_pair(width, height);

		constexpr auto cursor_position_ndc0 = GetNormalizedDeviceCoordinates(dvec2{0.f, 0.f}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc0.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc0.y, 1.f);

		constexpr auto cursor_position_ndc1 = GetNormalizedDeviceCoordinates(dvec2{0.f, height}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc1.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc1.y, -1.f);

		constexpr auto cursor_position_ndc2 = GetNormalizedDeviceCoordinates(dvec2{width, height}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc2.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc2.y, -1.f);

		constexpr auto cursor_position_ndc3 = GetNormalizedDeviceCoordinates(dvec2{width, 0.f}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc3.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc3.y, 1.f);

		constexpr auto cursor_position_ndc4 = GetNormalizedDeviceCoordinates(dvec2{width / 2.f, height / 2.f}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc4.x, 0.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc4.y, 0.f);

		constexpr auto cursor_position_ndc5 = GetNormalizedDeviceCoordinates(dvec2{-1.f, -1.f}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc5.x, -1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc5.y, 1.f);

		constexpr auto cursor_position_ndc6 = GetNormalizedDeviceCoordinates(dvec2{width + 1.f, height + 1.f}, window_dimensions);
		ASSERT_FLOAT_EQ(cursor_position_ndc6.x, 1.f);
		ASSERT_FLOAT_EQ(cursor_position_ndc6.y, -1.f);
	}

	TEST(ArcballTest, TestGetArcballPositionForCursorInsideUnitSphere) {
		constexpr auto x = 0.5f, y = .25f;
		constexpr vec2 cursor_position_ndc{x, y};
		const auto arcball_position = GetArcballPosition(cursor_position_ndc);
		ASSERT_FLOAT_EQ(arcball_position.x, x);
		ASSERT_FLOAT_EQ(arcball_position.y, y);
		ASSERT_FLOAT_EQ(arcball_position.z, .82915622f);
	}

	TEST(ArcballTest, TestGetArcballPositionForCursorOutsideUnitSphere) {
		constexpr auto x = .75f, y = .85f;
		constexpr vec2 cursor_position_ndc{x, y};
		const auto arcball_position = GetArcballPosition(cursor_position_ndc);
		ASSERT_FLOAT_EQ(arcball_position.x, .66162163f);
		ASSERT_FLOAT_EQ(arcball_position.y, .74983788f);
		ASSERT_FLOAT_EQ(arcball_position.z, 0.f);
	}
}
