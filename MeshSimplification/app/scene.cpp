#include "scene.h"

#include <algorithm>
#include <array>
#include <optional>

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

Scene::Scene(Window& window, ShaderProgram& shader_program)
	: window_{window},
	  shader_program_{shader_program},
	  mesh_{obj_loader::LoadMesh("models/bunny.obj")},
	  view_transform_{lookAt(kCamera.eye, kCamera.center, kCamera.up)} {

	window.set_on_key_press([this](const auto key_code) { HandleDiscreteKeyPress(key_code); });

	shader_program_.Enable();
	InitializeMesh();
	InitializePointLights();
}

void Scene::Render(const float delta_time) {

	HandleContinuousInput(delta_time);
	UpdateProjectionTransform();

	shader_program_.SetUniform("view_model_transform", view_transform_ * mesh_.model_transform());
	mesh_.Render();
}

void Scene::InitializeMesh() {

	mesh_.Scale(vec3{.25f});
	mesh_.Translate(vec3{.5f, -.9f, 0.f});

	constexpr auto kMeshMaterial = Material::FromType(MaterialType::kJade);
	shader_program_.SetUniform("material.ambient", kMeshMaterial.ambient());
	shader_program_.SetUniform("material.diffuse", kMeshMaterial.diffuse());
	shader_program_.SetUniform("material.specular", kMeshMaterial.specular());
	shader_program_.SetUniform("material.shininess", kMeshMaterial.shininess() * 128.f);
}

void Scene::InitializePointLights() {

	constexpr array kPointLights{
		PointLight{
			.position = vec4{1.f, 1.f, 1.f, 1.f},
			.color = vec3{1.f},
			.attenuation = vec3{0.f, 0.f, 1.f}
		},
		PointLight{
			.position = vec4{-1.f, 1.f, 2.f, 1.f},
			.color = vec3{1.f},
			.attenuation = vec3{0.f, 0.f, 1.f}
		}
	};

	for (size_t i = 0; i < kPointLights.size(); ++i) {
		const auto& [position, color, attenuation] = kPointLights[i];
		shader_program_.SetUniform(format("point_lights[{}].position", i), vec3{view_transform_ * position});
		shader_program_.SetUniform(format("point_lights[{}].color", i), color);
		shader_program_.SetUniform(format("point_lights[{}].attenuation", i), attenuation);
	}
}

void Scene::UpdateProjectionTransform() {

	static constexpr ViewFrustum kViewFrustrum{
		.field_of_view_y = radians(45.f),
		.z_near = .1f,
		.z_far = 100.f
	};

	static pair<int, int> prev_window_dimensions;
	const auto window_dimensions = window_.GetSize();

	if (const auto [width, height] = window_.GetSize(); width && height && window_dimensions != prev_window_dimensions) {
		const auto [field_of_view_y, z_near, z_far] = kViewFrustrum;
		const auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
		const auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
		shader_program_.SetUniform("projection_transform", projection_transform);
		prev_window_dimensions = window_dimensions;
	}
}

void Scene::HandleDiscreteKeyPress(const int key_code) {

	switch (key_code) {
		case GLFW_KEY_S: {
			mesh_ = mesh::Simplify(mesh_, .5f);
			break;
		}
		case GLFW_KEY_P: {
			static auto use_phong_shading = false;
			use_phong_shading = !use_phong_shading;
			shader_program_.SetUniform("use_phong_shading", use_phong_shading);
			break;
		}
		default:
			break;
	}
}

void Scene::HandleContinuousInput(const float delta_time) {

	static optional<dvec2> prev_cursor_position;
	const auto translate_step = 1.25f * delta_time;
	const auto scale_step = .75f * delta_time;

	if (window_.IsKeyPressed(GLFW_KEY_W)) {
		mesh_.Translate(vec3{0.f, translate_step, 0.f});
	} else if (window_.IsKeyPressed(GLFW_KEY_X)) {
		mesh_.Translate(vec3{0.f, -translate_step, 0.f});
	}

	if (window_.IsKeyPressed(GLFW_KEY_A)) {
		mesh_.Translate(vec3{-translate_step, 0.f, 0.f});
	} else if (window_.IsKeyPressed(GLFW_KEY_D)) {
		mesh_.Translate(vec3{translate_step, 0.f, 0.f});
	}

	if (window_.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && window_.IsKeyPressed(GLFW_KEY_EQUAL)) {
		mesh_.Scale(vec3{1.f + scale_step});
	} else if (window_.IsKeyPressed(GLFW_KEY_MINUS)) {
		mesh_.Scale(vec3{1.f - scale_step});
	}

	if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
		const auto cursor_position = window_.GetCursorPosition();
		if (prev_cursor_position) {
			if (const auto axis_and_angle = arcball::GetRotation(*prev_cursor_position, cursor_position, window_.GetSize())) {
				const auto& [view_rotation_axis, angle] = *axis_and_angle;
				const auto view_model_transform = view_transform_ * mesh_.model_transform();
				const auto model_rotation_axis = mat3{inverse(view_model_transform)} * view_rotation_axis;
				mesh_.Rotate(normalize(model_rotation_axis), angle);
			}
		}
		prev_cursor_position = cursor_position;
	} else if (prev_cursor_position.has_value()) {
		prev_cursor_position = nullopt;
	}
}
