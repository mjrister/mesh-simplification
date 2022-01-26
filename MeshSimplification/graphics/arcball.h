#pragma once

#include <optional>
#include <utility>

#include <glm/fwd.hpp>

namespace gfx::arcball {

/**
 * \brief Gets the axis and angle to rotate a mesh using changes in cursor position.
 * \param cursor_position_start The starting cursor position.
 * \param cursor_position_end The ending cursor position.
 * \param window_size The window width and height.
 * \return The axis (in camera space) and angle to rotate the mesh if the angle between the arcball positions of
 *         \p cursor_position_start and \p cursor_position_end is nonzero, otherwise \c std::nullopt.
 * \see docs/arcball.pdf for a detailed description of the arcball interface.
 */
std::optional<const std::pair<const glm::vec3, const float>> GetRotation(
	const glm::dvec2& cursor_position_start,
	const glm::dvec2& cursor_position_end,
	const std::pair<const int, const int>& window_size);
}
