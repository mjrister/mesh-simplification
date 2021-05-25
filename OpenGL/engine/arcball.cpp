#include "arcball.h"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>

#include "window.h"

namespace {

	/**
	 * \brief Gets the cursor position in normalized device coordinates (e.g., \f$(x,y) \in [-1, 1]\f$).
	 * \param cursor_position The cursor position in the window (e.g., \f$(x,y) \in [0, 1]\f$).
	 * \param window_size The window width and height.
	 * \return The cursor position in normalized device coordinates.
	 */
	constexpr glm::vec2 GetNormalizedDeviceCoordinates(
		const glm::dvec2& cursor_position, const std::pair<const std::int32_t, const std::int32_t>& window_size) {

		// normalize cursor position to [-1, 1] using clamp to handle cursor positions outside the window bounds
		const auto [width, height] = window_size;
		constexpr auto min = -1.f, max = 1.f;
		const auto x_ndc = std::clamp(static_cast<GLfloat>(cursor_position.x * 2.0 / width - 1.0), min, max);
		const auto y_ndc = std::clamp(static_cast<GLfloat>(cursor_position.y * 2.0 / height - 1.0), min, max);

		// because window coordinates start with (0,0) in the top-left corner which becomes (-1,-1) after normalization,
		// the y-coordinate needs to be negated to align with the OpenGL convention of the top-left residing at (-1,1)
		return {x_ndc, -y_ndc};
	}

	/**
	 * \brief Projects a cursor position onto the surface of the arcball.
	 * \param cursor_position_ndc The cursor position in normalized device coordinates.
	 * \return The cursor position on the arcball.
	 */
	glm::vec3 GetArcballPosition(const glm::vec2& cursor_position_ndc) {
		const auto x = cursor_position_ndc.x;
		const auto y = cursor_position_ndc.y;

		// compute z using the standard equation for a unit sphere (x^2 + y^2 + z^2 = 1)
		if (const auto c = x * x + y * y; c <= 1.f) {
			return glm::vec3{x, y, std::sqrt(1.f - c)};
		}

		return glm::normalize(glm::vec3{x, y, 0.f});
	}
}

std::optional<const std::pair<const glm::vec3, const GLfloat>> arcball::GetRotation(
	const glm::dvec2& cursor_position_start,
	const glm::dvec2& cursor_position_end,
	const std::pair<const std::int32_t, const std::int32_t>& window_size) {

	const auto cursor_position_start_ndc = GetNormalizedDeviceCoordinates(cursor_position_start, window_size);
	const auto cursor_position_end_ndc = GetNormalizedDeviceCoordinates(cursor_position_end, window_size);

	const auto arcball_position_start = GetArcballPosition(cursor_position_start_ndc);
	const auto arcball_position_end = GetArcballPosition(cursor_position_end_ndc);

	// use min to account for numerical issues where the dot product is greater than 1 causing acos to produce NaN
	const auto angle = std::acos(std::min<>(1.f, glm::dot(arcball_position_start, arcball_position_end)));

	if (static constexpr GLfloat epsilon = 1e-3f; angle > epsilon) {
		const auto axis = glm::cross(arcball_position_start, arcball_position_end);
		return std::make_pair(axis, angle);
	}

	return std::nullopt;
}
