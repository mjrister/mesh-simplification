#pragma once

#include <optional>
#include <utility>

#include <GL/gl3w.h>
#include <glm/fwd.hpp>

class Window;

namespace arcball {

	std::optional<const std::pair<const glm::vec3, const GLfloat>> GetRotation(
		const Window& window, const glm::dvec2& cursor_position_start, const glm::dvec2& cursor_position_end);
}
