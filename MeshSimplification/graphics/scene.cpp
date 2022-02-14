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

using namespace glm;
using namespace qem;
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
	.look_from = vec3{0.f, .5f, 2.f},
	.look_at = vec3{0.f},
	.up = vec3{0.f, 1.f, 0.f},
	.view_transform = lookAt(kCamera.look_from, kCamera.look_at, kCamera.up)
};

struct PointLight {
	vec3 position; // defined in view-space coordinates
	vec3 color;
} constexpr kPointLights[] = {
	{
		.position = vec3{1.f, 1.f, -.5f},
		.color = vec3{1.f},
	},
	{
		.position = vec3{-1.5f, 1.5f, -.5f},
		.color = vec3{1.f},
	},
	{
		.position = vec3{0.f, 2.f, -2.f},
		.color = vec3{1.f},
	}
};

void SetMaterial(ShaderProgram& shader_program) {
	const auto [ambient, diffuse, specular, shininess] = Material::FromType(MaterialType::kJade);
	shader_program.SetUniform("material.ambient", ambient);
	shader_program.SetUniform("material.diffuse", diffuse);
	shader_program.SetUniform("material.specular", specular);
	shader_program.SetUniform("material.shininess", shininess * 128.f);
}

void SetPointLights(ShaderProgram& shader_program) {
	constexpr auto kPointLightsSize = sizeof kPointLights / sizeof(PointLight);
	shader_program.SetUniform("point_lights_size", static_cast<int>(kPointLightsSize));

	for (size_t i = 0; i < kPointLightsSize; ++i) {
		const auto& [position, color] = kPointLights[i];
		shader_program.SetUniform(format("point_lights[{}].position", i), position);
		shader_program.SetUniform(format("point_lights[{}].color", i), color);
	}
}

void SetProjectionTransform(ShaderProgram& shader_program, const pair<int, int>& window_dimensions) {
	static pair<int, int> prev_window_dimensions;
	const auto [width, height] = window_dimensions;

	if (window_dimensions != prev_window_dimensions && width && height) {
		const auto [field_of_view_y, z_near, z_far] = kViewFrustrum;
		const auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
		const auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
		shader_program.SetUniform("projection_transform", projection_transform);
		prev_window_dimensions = window_dimensions;
	}
}

void SetViewTransforms(ShaderProgram& shader_program, const Mesh& mesh) {
	const auto view_model_transform = kCamera.view_transform * mesh.model_transform();
	shader_program.SetUniform("view_model_transform", view_model_transform);
	shader_program.SetUniform("normal_transform", inverse(transpose(view_model_transform)));
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

void HandleContinuousInput(const Window& window, Mesh& mesh, const float delta_time) {
	static optional<dvec2> prev_cursor_position;

	if (const auto cursor_position = window.CursorPosition(); window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
		if (prev_cursor_position) {
			const auto translate_step = .25f * delta_time;
			const auto cursor_delta = translate_step * static_cast<vec2>(cursor_position - *prev_cursor_position);
			const auto view_model_inv = inverse(kCamera.view_transform * mesh.model_transform());
			const auto translate = view_model_inv * vec4{cursor_delta.x, -cursor_delta.y, 0.f, 0.f};
			mesh.Translate(translate);
		}
		prev_cursor_position = cursor_position;
	} else if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (prev_cursor_position) {
			if (const auto axis_and_angle = arcball::GetRotation(*prev_cursor_position, cursor_position, window.Dimensions())) {
				const auto& [view_rotation_axis, angle] = *axis_and_angle;
				const auto view_model_inv = inverse(kCamera.view_transform * mesh.model_transform());
				const auto model_rotation_axis = normalize(view_model_inv * vec4{view_rotation_axis, 0.f});
				mesh.Rotate(model_rotation_axis, angle);
			}
		}
		prev_cursor_position = cursor_position;
	} else if (prev_cursor_position.has_value()) {
		prev_cursor_position = nullopt;
	}
}
}

Scene::Scene(Window* const window)
	: window_{window},
	  mesh_shader_program_{"shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl"},
	  mesh_{obj_loader::LoadMesh("models/bunny.obj", scale(translate(mat4{1.f}, vec3{.2f, -.25f, 0.f}), vec3{.3f}))} {

	window_->OnKeyPress([this](const auto key_code) {
		HandleDiscreteKeyPress(key_code, mesh_shader_program_, mesh_);
	});

	window_->OnScroll([this](const auto /*x_offset*/, const auto y_offset) {
		constexpr auto kScaleStep = .02f;
		const auto sign = static_cast<float>(y_offset > 0) - static_cast<float>(y_offset < 0);
		mesh_.Scale(vec3{1.f + sign * kScaleStep});
	});

	mesh_shader_program_.Enable([&] {
		SetMaterial(mesh_shader_program_);
		SetPointLights(mesh_shader_program_);
	});
}

void Scene::Render(const float delta_time) {

	mesh_shader_program_.Enable([&, this] {
		HandleContinuousInput(*window_, mesh_, delta_time);
		SetProjectionTransform(mesh_shader_program_, window_->Dimensions());
		SetViewTransforms(mesh_shader_program_, mesh_);
		mesh_.Render();
	});
}
