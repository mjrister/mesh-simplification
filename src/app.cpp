#include "app.h"  // NOLINT(build/include_subdir)

#include <optional>
#include <utility>

#include <glm/glm.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/mesh.h"
#include "graphics/scene.h"
#include "graphics/window.h"

namespace app {

namespace {

void HandleKeyEvent(const gfx::Window& window, gfx::Mesh& mesh, const int key, const int action) {
  if (action != GLFW_PRESS) return;

  switch (key) {
    case GLFW_KEY_ESCAPE:
      window.Close();
      break;
    case GLFW_KEY_S: {
      static constexpr auto kSimplificationRate = 0.5f;
      mesh = gfx::mesh::Simplify(mesh, kSimplificationRate);
      break;
    }
    default:
      break;
  }
}

void HandleCursorEvent(const gfx::Window& window, gfx::ArcCamera& camera, const float x, const float y) {
  static std::optional<glm::vec2> prev_cursor_position;
  const glm::vec2 cursor_position{x, y};

  if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    if (prev_cursor_position.has_value()) {
      static constexpr auto kRotationSpeed = 0.00390625f;
      const auto drag_direction = cursor_position - *prev_cursor_position;
      const auto rotation = kRotationSpeed * -drag_direction;
      camera.Rotate(rotation.x, rotation.y);
    }
    prev_cursor_position = cursor_position;
  } else if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    if (prev_cursor_position.has_value()) {
      static constexpr auto kTranslationSpeed = 0.001953125f;
      const auto drag_direction = cursor_position - *prev_cursor_position;
      const auto translation = kTranslationSpeed * glm::vec2{-drag_direction.x, drag_direction.y};
      camera.Translate(translation.x, translation.y, 0.0f);
    }
    prev_cursor_position = cursor_position;
  } else {
    prev_cursor_position = std::nullopt;
  }
}

void HandleScrollEvent(gfx::ArcCamera& camera, const float y) {
  static constexpr auto kZoomSpeed = 0.015625f;
  camera.Zoom(kZoomSpeed * -y);
}

}  // namespace

void Run(const char* const app_name, const std::pair<int, int> window_size, const std::pair<int, int> opengl_version) {
  gfx::Window window{app_name, window_size, opengl_version};
  gfx::Scene scene{window, "assets/models/bunny.obj"};
  auto& camera = scene.camera();
  auto& mesh = scene.mesh();

  window.OnKeyEvent([&window, &mesh](const auto key, const auto action) { HandleKeyEvent(window, mesh, key, action); });
  window.OnCursorEvent([&window, &camera](const auto x, const auto y) { HandleCursorEvent(window, camera, x, y); });
  window.OnScrollEvent([&camera](const auto y_offset) { HandleScrollEvent(camera, y_offset); });

  // NOLINTBEGIN(*-magic-numbers)
  mesh.Translate(glm::vec3{0.2f, -0.3f, 0.0f});
  mesh.Rotate(glm::vec3{1.0f, 0.0f, 0.0f}, glm::radians(10.0f));
  mesh.Scale(glm::vec3{0.35f});
  // NOLINTEND(*-magic-numbers)

  while (!window.IsClosed()) {
    window.Update();
    scene.Render();
  }
}

}  // namespace app
