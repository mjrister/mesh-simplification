#include "scene.h"

#include <algorithm>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/arcball.h"
#include "graphics/material.h"
#include "graphics/obj_loader.h"

using namespace app;
using namespace geometry;
using namespace gfx;
using namespace glm;
using namespace std;

namespace {

struct ViewFrustrum {
	float field_of_view_y;
	float z_near;
	float z_far;
} constexpr kViewFrustrum = {
	.field_of_view_y = radians(45.f),
	.z_near = .1f,
	.z_far = 100.f
};

struct Camera {
	vec3 look_from;
	vec3 look_at;
	vec3 up;
	mat4 view_transform;
} const kCamera = {
	.look_from = vec3{0., 0., 2.f},
	.look_at = vec3{0.f},
	.up = vec3{0.f, 1.f, 0.f},
	.view_transform = lookAt(kCamera.look_from, kCamera.look_at, kCamera.up)
};

struct PointLight {
	vec4 position;
	vec3 color;
	vec3 attenuation;
} constexpr kPointLights[] = {
	{
		.position = vec4{1.f, 1.f, 1.f, 1.f},
		.color = vec3{1.f},
		.attenuation = vec3{0.f, 0.f, 1.f}
	},
	{
		.position = vec4{-1.f, 1.f, 2.f, 1.f},
		.color = vec3{1.f},
		.attenuation = vec3{0.f, 0.f, 1.f}
	}
};

void InitializeMesh(ShaderProgram& shader_program, Mesh& mesh) {
	constexpr auto kMeshMaterial = Material::FromType(MaterialType::kJade);

	shader_program.SetUniform("material.ambient", kMeshMaterial.ambient());
	shader_program.SetUniform("material.diffuse", kMeshMaterial.diffuse());
	shader_program.SetUniform("material.specular", kMeshMaterial.specular());
	shader_program.SetUniform("material.shininess", kMeshMaterial.shininess() * 128.f);

	mesh.Scale(vec3{.25f});
	mesh.Translate(vec3{.5f, -.9f, 0.f});
}

void InitializePointLights(ShaderProgram& shader_program) {
	constexpr auto kPointLightsSize = sizeof kPointLights / sizeof(PointLight);

	for (size_t i = 0; i < kPointLightsSize; ++i) {
		const auto& [position, color, attenuation] = kPointLights[i];
		shader_program.SetUniform(format("point_lights[{}].position", i), vec3{kCamera.view_transform * position});
		shader_program.SetUniform(format("point_lights[{}].color", i), color);
		shader_program.SetUniform(format("point_lights[{}].attenuation", i), attenuation);
	}
}

void UpdateProjectionTransform(const Window& window, ShaderProgram& shader_program) {
	static pair<int, int> prev_window_dimensions;
	const auto window_dimensions = window.GetDimensions();

	if (const auto [width, height] = window_dimensions; width && height && window_dimensions != prev_window_dimensions) {
		const auto [field_of_view_y, z_near, z_far] = kViewFrustrum;
		const auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
		const auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
		shader_program.SetUniform("projection_transform", projection_transform);
		prev_window_dimensions = window_dimensions;
	}
}

void HandleDiscreteKeyPress(const int key_code, ShaderProgram& shader_program, Mesh& mesh) {

	switch (key_code) {
		case GLFW_KEY_S:
			mesh = mesh::Simplify(mesh, .5f);
			break;
		case GLFW_KEY_P: {
			static auto use_phong_shading = false;
			use_phong_shading = !use_phong_shading;
			shader_program.SetUniform("use_phong_shading", use_phong_shading);
			break;
		}
		default:
			break;
	}
}

void HandleContinuousInput(const Window& window, const float delta_time, Mesh& mesh) {
	static optional<dvec2> prev_cursor_position;
	const auto translate_step = 1.25f * delta_time;
	const auto scale_step = .75f * delta_time;

	if (window.IsKeyPressed(GLFW_KEY_LEFT)) {
		mesh.Translate(vec3{-translate_step, 0.f, 0.f});
	} else if (window.IsKeyPressed(GLFW_KEY_RIGHT)) {
		mesh.Translate(vec3{translate_step, 0.f, 0.f});
	}

	if (window.IsKeyPressed(GLFW_KEY_UP)) {
		mesh.Translate(vec3{0.f, translate_step, 0.f});
	} else if (window.IsKeyPressed(GLFW_KEY_DOWN)) {
		mesh.Translate(vec3{0.f, -translate_step, 0.f});
	}

	if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && window.IsKeyPressed(GLFW_KEY_EQUAL)) {
		mesh.Scale(vec3{1.f + scale_step});
	} else if (window.IsKeyPressed(GLFW_KEY_MINUS)) {
		mesh.Scale(vec3{1.f - scale_step});
	}

	if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
		const auto cursor_position = window.GetCursorPosition();

		if (prev_cursor_position) {
			const auto window_dimensions = window.GetDimensions();

			if (const auto axis_and_angle = arcball::GetRotation(*prev_cursor_position, cursor_position, window_dimensions)) {
				const auto& [view_rotation_axis, angle] = *axis_and_angle;
				const auto view_model_transform = kCamera.view_transform * mesh.model_transform();
				const auto model_rotation_axis = mat3{inverse(view_model_transform)} * view_rotation_axis;
				mesh.Rotate(normalize(model_rotation_axis), angle);
			}
		}
		prev_cursor_position = cursor_position;
	} else if (prev_cursor_position.has_value()) {
		prev_cursor_position = nullopt;
	}
}
}

Scene::Scene(Window* const window, ShaderProgram* const shader_program)
	: window_{window}, shader_program_{shader_program}, mesh_{obj_loader::LoadMesh("models/bunny.obj")} {

	window_->on_key_press([this](const auto key_code) { HandleDiscreteKeyPress(key_code, *shader_program_, mesh_); });
	shader_program_->Enable();

	InitializeMesh(*shader_program_, mesh_);
	InitializePointLights(*shader_program_);
}

void Scene::Render(const float delta_time) {

	HandleContinuousInput(*window_, delta_time, mesh_);
	UpdateProjectionTransform(*window_, *shader_program_);

	// Generally, normals should be transformed by the upper 3x3 inverse transpose of the view model matrix. In this context,
	// it is sufficient to use the view-model matrix to transform normals because meshes are only transformed by rotations
	// and translations (which are orthogonal matrices with the property that their inverse is equal to their transpose) in
	// addition to uniform scaling which is undone when the transformed normal is renormalized in the vertex shader.
	const auto view_model_transform = kCamera.view_transform * mesh_.model_transform();
	shader_program_->SetUniform("view_model_transform", view_model_transform);
	shader_program_->SetUniform("normal_transform", mat3{view_model_transform});

	mesh_.Render();
}
