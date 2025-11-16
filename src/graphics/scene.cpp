#include "graphics/scene.h"

#include <array>

#include <glm/glm.hpp>

#include "graphics/material.h"
#include "graphics/obj_loader.h"
#include "graphics/shader_program.h"
#include "graphics/window.h"

namespace gfx {

namespace {

struct PointLight {
  glm::vec3 position{0.0f};  // defined in view-space coordinates
  glm::vec3 color{0.0f};
};

ArcCamera CreateCamera(const float aspect_ratio) {
  static constexpr glm::vec3 kPosition{0.0f, 0.0f, 2.0f};
  static constexpr glm::vec3 kTarget{0.0f};

  return ArcCamera{kPosition,
                   kTarget,
                   ViewFrustum{
                       // NOLINTBEGIN(*-magic-numbers)
                       .field_of_view_y = glm::radians(45.0f),
                       .aspect_ratio = aspect_ratio,
                       .z_near = 0.1f,
                       .z_far = 1.0e6f
                       // NOLINTEND(*-magic-numbers)
                   }};
}

constexpr std::array<PointLight, 3> CreatePointLights() {
  constexpr glm::vec3 kWhiteColor{1.0f};

  return std::array{
      // NOLINTBEGIN(*-magic-numbers)
      PointLight{.position = glm::vec3{1.0f, 1.0f, 0.0f}, .color = kWhiteColor},
      PointLight{.position = glm::vec3{-1.0f, 0.0f, 1.0f}, .color = kWhiteColor},
      PointLight{.position = glm::vec3{0.0f, 3.0f, -2.0f}, .color = kWhiteColor}
      // NOLINTEND(*-magic-numbers)
  };
}

void SetPointLights(const ShaderProgram& shader_program) {
  static constexpr auto kPointLights = CreatePointLights();
  shader_program.SetUniform("point_lights_size", static_cast<GLint>(kPointLights.size()));

  for (std::size_t i = 0; i < kPointLights.size(); ++i) {
    const auto& [position, color] = kPointLights[i];  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    shader_program.SetUniform(std::format("point_lights[{}].position", i), position);
    shader_program.SetUniform(std::format("point_lights[{}].color", i), color);
  }
}

void SetMaterial(const ShaderProgram& shader_program) {
  const auto [ambient, diffuse, specular, shininess] = Material::FromType(Material::Type::kJade);
  shader_program.SetUniform("material.ambient", ambient);
  shader_program.SetUniform("material.diffuse", diffuse);
  shader_program.SetUniform("material.specular", specular);
  shader_program.SetUniform("material.shininess", shininess);
}

}  // namespace

Scene::Scene(const Window& window, const std::filesystem::path& obj_filepath)
    : camera_{CreateCamera(window.GetAspectRatio())},
      mesh_{obj_loader::LoadMesh(obj_filepath)},
      shader_program_{"assets/shaders/mesh_vertex.glsl", "assets/shaders/mesh_fragment.glsl"} {
  shader_program_.Enable();
  SetPointLights(shader_program_);
  SetMaterial(shader_program_);
}

void Scene::Render() const {
  const auto model_view_transform = camera_.GetViewTransform() * mesh_.model_transform();
  shader_program_.SetUniform("model_view_transform", model_view_transform);
  shader_program_.SetUniform("projection_transform", camera_.GetProjectionTransform());

  static constexpr auto kDefaultClearColorValue = 0.1f;
  glClearColor(kDefaultClearColorValue, kDefaultClearColorValue, kDefaultClearColorValue, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mesh_.Render();
}

}  // namespace gfx
