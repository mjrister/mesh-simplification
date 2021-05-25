#pragma once

#include <optional>
#include <utility>

#include <GL/gl3w.h>
#include <glm/fwd.hpp>

class Window;

namespace arcball {

	/**
	 * \brief Gets the axis and angle to rotate a mesh using changes in cursor position.
	 * \param cursor_position_start The starting cursor position.
	 * \param cursor_position_end The ending cursor position.
	 * \param window_dimensions The window width and height.
	 * \return An optional value containing the axis and angle to rotate the mesh if the angle between
	 *         \p cursor_position_start and \p cursor_position_end is nonzero.
	 * \see docs/arcball.pdf
	 */
	std::optional<const std::pair<const glm::vec3, const GLfloat>> GetRotation(
		const glm::dvec2& cursor_position_start,
		const glm::dvec2& cursor_position_end,
		const std::pair<const std::int32_t, const std::int32_t>& window_dimensions);
}
