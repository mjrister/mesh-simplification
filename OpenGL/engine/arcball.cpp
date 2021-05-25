#include "arcball.h"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>

#include "window.h"

namespace {
	constexpr glm::vec2 GetNormalizedDeviceCoordinates(
		const glm::dvec2& cursor_position, const std::int32_t window_width, const std::int32_t window_height) {

		const auto x_ndc = static_cast<GLfloat>(cursor_position.x * 2.0 / window_width - 1.0);
		const auto y_ndc = static_cast<GLfloat>(cursor_position.y * 2.0 / window_height - 1.0);

		return {std::clamp(x_ndc, -1.f, 1.f), std::clamp(-y_ndc, -1.f, 1.f)};
	}

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
	const Window& window, const glm::dvec2& cursor_position_start, const glm::dvec2& cursor_position_end) {

	const auto [width, height] = window.Size();
	const auto cursor_position_start_ndc = GetNormalizedDeviceCoordinates(cursor_position_start, width, height);
	const auto cursor_position_end_ndc = GetNormalizedDeviceCoordinates(cursor_position_end, width, height);

	const auto arcball_position_start = GetArcballPosition(cursor_position_start_ndc);
	const auto arcball_position_end = GetArcballPosition(cursor_position_end_ndc);
	const auto angle = std::acos(std::min<>(1.f, glm::dot(arcball_position_start, arcball_position_end)));

	if (static constexpr GLfloat epsilon = 1e-3f; angle > epsilon) {
		const auto axis = glm::cross(arcball_position_start, arcball_position_end);
		return std::make_pair(axis, angle);
	}

	return std::nullopt;
}
