#include "graphics/arcball.h"

#include <algorithm>
#include <cmath>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

#include "app/window.h"

using namespace app;
using namespace gfx;
using namespace glm;
using namespace std;

namespace {

	/**
	 * \brief Gets the cursor position in normalized device coordinates (e.g., \f$(x,y) \in [-1, 1]\f$).
	 * \param cursor_position The cursor position in the window.
	 * \param window_size The window width and height.
	 * \return The cursor position in normalized device coordinates.
	 */
	constexpr vec2 GetNormalizedDeviceCoordinates(
		const dvec2& cursor_position, const pair<const int32_t, const int32_t>& window_size) {

		// normalize cursor position to [-1, 1] using clamp to handle cursor positions outside the window bounds
		const auto [width, height] = window_size;
		constexpr auto min = -1., max = 1.;
		const auto x_ndc = std::clamp(cursor_position.x * 2. / width - 1., min, max);
		const auto y_ndc = std::clamp(cursor_position.y * 2. / height - 1., min, max);

		// because window coordinates start with (0,0) in the top-left corner which becomes (-1,-1) after normalization,
		// the y-coordinate needs to be negated to align with the OpenGL convention of the top-left residing at (-1,1)
		return {x_ndc, -y_ndc};
	}

	/**
	 * \brief Projects a cursor position onto the surface of the arcball.
	 * \param cursor_position_ndc The cursor position in normalized device coordinates.
	 * \return The cursor position on the arcball.
	 */
	vec3 GetArcballPosition(const vec2& cursor_position_ndc) {
		const auto x = cursor_position_ndc.x;
		const auto y = cursor_position_ndc.y;

		// compute z using the standard equation for a unit sphere (x^2 + y^2 + z^2 = 1)
		if (const auto c = x * x + y * y; c <= 1.f) {
			return vec3{x, y, sqrt(1.f - c)};
		}

		// get nearest position on the arcball
		return normalize(vec3{x, y, 0.f});
	}
}

optional<const pair<const vec3, const float>> arcball::GetRotation(
	const dvec2& cursor_position_start,
	const dvec2& cursor_position_end,
	const pair<const int32_t, const int32_t>& window_size) {

	const auto cursor_position_start_ndc = GetNormalizedDeviceCoordinates(cursor_position_start, window_size);
	const auto cursor_position_end_ndc = GetNormalizedDeviceCoordinates(cursor_position_end, window_size);

	const auto arcball_position_start = GetArcballPosition(cursor_position_start_ndc);
	const auto arcball_position_end = GetArcballPosition(cursor_position_end_ndc);

	// use min to account for numerical issues where the dot product is greater than 1 causing acos to produce NaN
	const auto angle = acos(std::min<>(1.f, dot(arcball_position_start, arcball_position_end)));

	if (static constexpr auto epsilon = 1e-3f; angle > epsilon) {
		const auto axis = cross(arcball_position_start, arcball_position_end);
		return make_pair(axis, angle);
	}

	return nullopt;
}
