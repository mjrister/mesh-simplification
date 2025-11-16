#include "graphics/arc_camera.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gfx {

namespace {

SphericalCoordinates ToSphericalCoordinates(const glm::vec3& cartesian_coordinates) {
  const auto radius = glm::length(cartesian_coordinates);
  return radius == 0.0f ? SphericalCoordinates{.radius = 0.0f, .theta = 0.0f, .phi = 0.0f}
                        : SphericalCoordinates{.radius = radius,
                                               .theta = std::atan2(cartesian_coordinates.x, cartesian_coordinates.z),
                                               .phi = std::asin(-cartesian_coordinates.y / radius)};
}

glm::vec3 ToCartesianCoordinates(const SphericalCoordinates& spherical_coordinates) {
  const auto [radius, theta, phi] = spherical_coordinates;
  const auto cos_phi = std::cos(phi);
  const auto x = radius * std::sin(theta) * cos_phi;
  const auto y = radius * std::sin(-phi);
  const auto z = radius * std::cos(theta) * cos_phi;
  return glm::vec3{x, y, z};
}

}  // namespace

ArcCamera::ArcCamera(const glm::vec3& position, const glm::vec3& target, const ViewFrustum& view_frustum)
    : position_{ToSphericalCoordinates(position - target)}, target_{target}, view_frustum_{view_frustum} {}

glm::mat4 ArcCamera::GetViewTransform() const {
  static constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
  const auto cartesian_position = target_ + ToCartesianCoordinates(position_);
  return glm::lookAt(cartesian_position, target_, kUp);
}

glm::mat4 ArcCamera::GetProjectionTransform() const {
  const auto [field_of_view_y, aspect_ratio, z_near, z_far] = view_frustum_;
  return glm::perspective(field_of_view_y, aspect_ratio, z_near, z_far);
}

void ArcCamera::Translate(const float dx, const float dy, const float dz) {
  const glm::mat3 orientation = GetViewTransform();
  target_ += glm::vec3{dx, dy, dz} * orientation;  // NOLINT(whitespace/braces)
}

void ArcCamera::Rotate(const float theta, const float phi) {
  static constexpr auto kThetaMax = glm::two_pi<float>();
  static constexpr auto kPhiMax = glm::radians(89.0f);
  position_.theta = std::fmod(position_.theta + theta, kThetaMax);
  position_.phi = std::clamp(position_.phi + phi, -kPhiMax, kPhiMax);
}

void ArcCamera::Zoom(const float rate) {
  static constexpr auto kEpsilon = std::numeric_limits<float>::epsilon();
  position_.radius = std::max((1.0f - rate) * position_.radius, kEpsilon);
}

}  // namespace gfx
