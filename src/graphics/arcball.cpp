#include "graphics/arcball.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace gfx {

namespace {

/**
 * @brief Gets the cursor position in normalized device coordinates.
 * @param cursor_position The cursor position in the window.
 * @param window_dimensions The window width and height.
 * @return The cursor position in normalized device coordinates.
 */
glm::vec2 GetNormalizedDeviceCoordinates(const glm::vec2& cursor_position,
                                         const std::pair<int, int>& window_dimensions) {
  // normalize cursor position to [-1, 1] using clamp to handle cursor positions outside the window bounds
  static constexpr auto kMinCoordinateValue = -1.0f;
  static constexpr auto kMaxCoordinateValue = 1.0f;
  const auto& [width, height] = window_dimensions;
  const auto x_ndc = std::clamp(cursor_position.x * 2.0f / width - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);
  const auto y_ndc = std::clamp(cursor_position.y * 2.0f / height - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);

  // because window coordinates start with (0,0) in the top-left corner which becomes (-1,-1) after normalization,
  // the y-coordinate needs to be negated to align with the OpenGL convention of the top-left residing at (-1,1)
  return glm::vec2{x_ndc, -y_ndc};
}

/**
 * @brief Projects a cursor position onto the surface of the arcball.
 * @param cursor_position_ndc The cursor position in normalized device coordinates.
 * @return The cursor position on the arcball.
 */
glm::vec3 GetArcballPosition(const glm::vec2& cursor_position_ndc) {
  const auto x = cursor_position_ndc.x;
  const auto y = cursor_position_ndc.y;

  // compute z using the standard equation for a unit sphere (x^2 + y^2 + z^2 = 1)
  if (const auto c = x * x + y * y; c <= 1.0f) {
    return glm::vec3{x, y, sqrt(1.0f - c)};
  }

  // get nearest position on the arcball
  return glm::normalize(glm::vec3{x, y, 0.0f});
}

}  // namespace

std::optional<std::pair<glm::vec3, float>> arcball::GetRotation(const glm::vec2& cursor_position_start,
                                                                const glm::vec2& cursor_position_end,
                                                                const std::pair<int, int>& window_dimensions) {
  const auto cursor_position_start_ndc = GetNormalizedDeviceCoordinates(cursor_position_start, window_dimensions);
  const auto cursor_position_end_ndc = GetNormalizedDeviceCoordinates(cursor_position_end, window_dimensions);

  const auto arcball_position_start = GetArcballPosition(cursor_position_start_ndc);
  const auto arcball_position_end = GetArcballPosition(cursor_position_end_ndc);

  // use min to account for numerical issues where the dot product is greater than 1 causing acos to produce NaN
  const auto angle = std::acos(std::min<>(1.0f, glm::dot(arcball_position_start, arcball_position_end)));

  if (static constexpr auto kEpsilon = 1e-3f; angle > kEpsilon) {
    const auto axis = glm::cross(arcball_position_start, arcball_position_end);
    return std::make_pair(axis, angle);
  }

  return std::nullopt;
}

}  // namespace gfx
