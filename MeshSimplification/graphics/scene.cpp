#include "graphics/scene.h"

#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/arcball.h"
#include "graphics/material.h"
#include "graphics/obj_loader.h"
#include "graphics/shader_program.h"
#include "graphics/window.h"

namespace {

struct Camera {
  glm::vec3 look_from;
  glm::vec3 look_at;
  glm::vec3 up;
  glm::mat4 view_transform;
} const kCamera = {
    .look_from = glm::vec3{0.f, .4f, 2.f},
    .look_at = glm::vec3{0.f},
    .up = glm::vec3{0.f, 1.f, 0.f},
    .view_transform = lookAt(kCamera.look_from, kCamera.look_at, kCamera.up),
};

struct ViewFrustum {
  float field_of_view_y;
  float z_near;
  float z_far;
} constexpr kViewFrustum = {
    .field_of_view_y = glm::radians(45.0f),
    .z_near = 0.1f,
    .z_far = 100.0f,
};

struct PointLight {
  glm::vec3 position;  // defined in view-space coordinates
  glm::vec3 color;
  glm::vec3 attenuation;
} constexpr kPointLights[] = {
    {
        .position = glm::vec3{1.0f, 1.0f, 0.0f},
        .color = glm::vec3{1.0f},
        .attenuation = glm::vec3{0.0f, 0.0f, 1.0f},
    },
    {
        .position = glm::vec3{-1.0f, 0.0f, 1.0f},
        .color = glm::vec3{1.0f},
        .attenuation = glm::vec3{0.0f, 0.0f, 1.0f},
    },
    {
        .position = glm::vec3{0.0f, 3.0f, -2.0f},
        .color = glm::vec3{1.0f},
        .attenuation = glm::vec3{0.0f, 0.0f, 1.0f},
    },
};

void SetMaterial(qem::ShaderProgram& shader_program) {
  const auto [ambient, diffuse, specular, shininess] = qem::Material::FromType(qem::Material::Type::kJade);
  shader_program.SetUniform("material.ambient", ambient);
  shader_program.SetUniform("material.diffuse", diffuse);
  shader_program.SetUniform("material.specular", specular);
  shader_program.SetUniform("material.shininess", shininess * 128.0f);
}

void SetPointLights(qem::ShaderProgram& shader_program) {
  constexpr int kPointLightsSize = sizeof kPointLights / sizeof(PointLight);
  shader_program.SetUniform("point_lights_size", kPointLightsSize);

  for (auto i = 0; i < kPointLightsSize; ++i) {
    const auto& [position, color, attenuation] = kPointLights[i];
    shader_program.SetUniform(std::format("point_lights[{}].position", i), position);
    shader_program.SetUniform(std::format("point_lights[{}].color", i), color);
    shader_program.SetUniform(std::format("point_lights[{}].attenuation", i), attenuation);
  }
}

void SetViewTransforms(const qem::Window& window, const qem::Mesh& mesh, qem::ShaderProgram& shader_program) {
  static auto prev_aspect_ratio = 0.0f;

  if (const auto aspect_ratio = window.GetAspectRatio(); prev_aspect_ratio != aspect_ratio && aspect_ratio > 0.0f) {
    const auto [field_of_view_y, z_near, z_far] = kViewFrustum;
    const auto projection_transform = glm::perspective(field_of_view_y, aspect_ratio, z_near, z_far);
    shader_program.SetUniform("projection_transform", projection_transform);
    prev_aspect_ratio = aspect_ratio;
  }

  const auto model_view_transform = kCamera.view_transform * mesh.model_transform();
  shader_program.SetUniform("model_view_transform", model_view_transform);
}

void HandleMouseInput(const qem::Window& window, qem::Mesh& mesh, const float delta_time) {
  static std::optional<glm::dvec2> prev_cursor_position;

  if (const auto cursor_position = window.GetCursorPosition(); window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    if (prev_cursor_position.has_value()) {
      const auto axis_angle = qem::arcball::GetRotation(*prev_cursor_position, cursor_position, window.GetSize());
      if (axis_angle.has_value()) {
        const auto rotation_speed = 256.0f * delta_time;
        const auto& [view_rotation_axis, angle] = *axis_angle;
        const auto view_model_inv = glm::inverse(kCamera.view_transform * mesh.model_transform());
        const auto model_rotation_axis = glm::normalize(view_model_inv * glm::vec4{view_rotation_axis, 0.f});
        mesh.Rotate(model_rotation_axis, rotation_speed * angle);
      }
    }
    prev_cursor_position = cursor_position;
  } else if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    if (prev_cursor_position.has_value()) {
      const auto translation_speed = 0.15f * delta_time;
      const auto cursor_delta = static_cast<glm::vec2>(cursor_position - *prev_cursor_position);
      const auto view_model_inv = glm::inverse(kCamera.view_transform * mesh.model_transform());
      mesh.Translate(view_model_inv * translation_speed * glm::vec4{cursor_delta.x, -cursor_delta.y, 0.0f, 0.0f});
    }
    prev_cursor_position = cursor_position;
  } else if (prev_cursor_position.has_value()) {
    prev_cursor_position = std::nullopt;
  }
}
}

qem::Scene::Scene(Window* const window)
    : window_{window},
      mesh_{obj_loader::LoadMesh("models/bunny.obj")},
      shader_program_{"shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl"} {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  window_->OnKeyPress([this](const auto key_code) {
    if (key_code == GLFW_KEY_S) {
      mesh_ = mesh::Simplify(mesh_, 0.5f);
    }
  });

  window_->OnScroll([this](const auto /*x_offset*/, const auto y_offset) {
    constexpr auto kScaleStep = 0.02f;
    const auto sign = static_cast<float>(y_offset > 0) - static_cast<float>(y_offset < 0);
    mesh_.Scale(glm::vec3{1.0f + sign * kScaleStep});
  });

  shader_program_.Enable();
  SetMaterial(shader_program_);
  SetPointLights(shader_program_);

  mesh_.Translate(kCamera.look_at + glm::vec3{.2f, -.25f, 0.f});
  mesh_.Scale(glm::vec3{0.35f});
}

void qem::Scene::Render(const float delta_time) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  HandleMouseInput(*window_, mesh_, delta_time);
  SetViewTransforms(*window_, mesh_, shader_program_);

  mesh_.Render();
}
