#include "arcball.h"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>

#include "window.h"

namespace {

	/**
	 * \brief Gets the cursor position in normalized device coordinates (e.g., \f$(x,y) \in [-1, 1]\f$).
	 * \param cursor_position The cursor position in the window (e.g., \f$(x,y) \in [0, 1]\f$).
	 * \param window_dimensions The window width and height.
	 * \return The cursor position in normalized device coordinates.
	 */
	constexpr glm::vec2 GetNormalizedDeviceCoordinates(
		const glm::dvec2& cursor_position, const std::pair<const std::int32_t, const std::int32_t>& window_dimensions) {

		const auto [width, height] = window_dimensions;
		const auto x_ndc = static_cast<GLfloat>(cursor_position.x * 2.0 / width - 1.0);
		const auto y_ndc = static_cast<GLfloat>(cursor_position.y * 2.0 / height - 1.0);

		return {std::clamp(x_ndc, -1.f, 1.f), std::clamp(-y_ndc, -1.f, 1.f)};
	}

	/**
	 * \brief Gets the position of the cursor on the arcball.
	 * \param cursor_position_ndc The cursor position in normalized device coordinates.
	 * \return The cursor position on the arcball.
	 */
	glm::vec3 GetArcballPosition(const glm::vec2& cursor_position_ndc) {
		const auto x = cursor_position_ndc.x;
		const auto y = cursor_position_ndc.y;

		if (const auto c = x * x + y * y; c <= 1.f) {
			return glm::vec3{x, y, std::sqrt(1.f - c)};
		}

		return glm::normalize(glm::vec3{x, y, 0.f});
	}
}

std::optional<const std::pair<const glm::vec3, const GLfloat>> arcball::GetRotation(
	const glm::dvec2& cursor_position_start,
	const glm::dvec2& cursor_position_end,
	const std::pair<const std::int32_t, const std::int32_t>& window_dimensions) {

	const auto cursor_position_start_ndc = GetNormalizedDeviceCoordinates(cursor_position_start, window_dimensions);
	const auto cursor_position_end_ndc = GetNormalizedDeviceCoordinates(cursor_position_end, window_dimensions);

	const auto arcball_position_start = GetArcballPosition(cursor_position_start_ndc);
	const auto arcball_position_end = GetArcballPosition(cursor_position_end_ndc);
	const auto angle = std::acos(std::min<>(1.f, glm::dot(arcball_position_start, arcball_position_end)));

	if (static constexpr GLfloat epsilon = 1e-3f; angle > epsilon) {
		const auto axis = glm::cross(arcball_position_start, arcball_position_end);
		return std::make_pair(axis, angle);
	}

	return std::nullopt;
}
