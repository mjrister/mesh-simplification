#include "arcball.h"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>

#include "window.h"

namespace {
	constexpr glm::vec2 GetNormalizedCursorPosition(
		const glm::dvec2& cursor_position, const std::int32_t window_width, const std::int32_t window_height) {

		const auto x_norm = static_cast<GLfloat>(cursor_position.x * 2.0 / window_width - 1.0);
		const auto y_norm = static_cast<GLfloat>(cursor_position.y * 2.0 / window_height - 1.0);

		return {std::clamp(x_norm, -1.f, 1.f), std::clamp(-y_norm, -1.f, 1.f)};
	}

	glm::vec3 GetArcballPosition(const glm::vec2& cursor_position_norm) {
		const auto x = cursor_position_norm.x;
		const auto y = cursor_position_norm.y;

		if (const auto c = x * x + y * y; c <= 1.f) {
			return glm::vec3{x, y, std::sqrt(1.f - c)};
		}

		return glm::normalize(glm::vec3{x, y, 0.f});
	}
}

std::optional<const std::pair<const glm::vec3, const GLfloat>> arcball::GetRotation(
	const Window& window, const glm::dvec2& cursor_position, const glm::dvec2& prev_cursor_position) {

	const auto [width, height] = window.Size();
	const auto cursor_position_norm = GetNormalizedCursorPosition(cursor_position, width, height);
	const auto prev_cursor_position_norm = GetNormalizedCursorPosition(prev_cursor_position, width, height);

	const auto arcball_position = GetArcballPosition(cursor_position_norm);
	const auto prev_arcball_position = GetArcballPosition(prev_cursor_position_norm);
	const auto angle = std::acos(std::min<>(1.f, glm::dot(prev_arcball_position, arcball_position)));

	if (static constexpr GLfloat epsilon = 1e-3f; angle > epsilon) {
		const auto axis = glm::cross(prev_arcball_position, arcball_position);
		return std::make_pair(axis, angle);
	}

	return std::nullopt;
}
