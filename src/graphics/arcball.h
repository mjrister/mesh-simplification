#ifndef GRAPHICS_ARCBALL_H_
#define GRAPHICS_ARCBALL_H_

#include <optional>
#include <utility>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace gfx::arcball {

/** @brief A structure representing an arcball rotation. */
struct Rotation {
  /** @brief The axis to rotate about. */
  glm::vec3 axis{0.0f};

  /** @brief The amount to rotate in radians. */
  float angle = 0.0f;
};

/**
 * @brief Gets a rotation from a cursor movement using the arcball interface.
 * @param cursor_position_start The starting cursor position.
 * @param cursor_position_end The ending cursor position.
 * @param window_size The window width and height.
 * @return The axis (in camera space) and angle to rotate the mesh if the angle between the arcball positions of
 *         @p cursor_position_start and @p cursor_position_end is nonzero, otherwise @c std::nullopt.
 * @see "A User Interface for Specifying Three-Dimensional Orientation Using a Mouse" docs/arcball.pdf
 */
std::optional<Rotation> GetRotation(const glm::vec2& cursor_position_start,
                                    const glm::vec2& cursor_position_end,
                                    const std::pair<int, int>& window_size);

}  // namespace gfx::arcball

#endif  // GRAPHICS_ARCBALL_H_
