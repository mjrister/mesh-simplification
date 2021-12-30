#include "scene.h"

#include <algorithm>
#include <optional>

#include "geometry/mesh_simplifier.h"
#include "graphics/arcball.h"
#include "graphics/obj_loader.h"

using namespace app;
using namespace geometry;
using namespace gfx;
using namespace glm;
using namespace std;

namespace {
	bool use_phong_shading = false;
	mat4 view_transform;
	pair<int32_t, int32_t> prev_window_dimensions;
}

Scene::Scene(Window& window, ShaderProgram& shader_program)
	: window_{window},
	  shader_program_{shader_program} {

	window.OnKeyPress([this](const auto key_code) { HandleDiscreteKeyPress(key_code); });

	auto mesh = obj_loader::LoadMesh("models/bunny.obj");
	mesh.Scale(vec3{.25f});
	mesh.Translate(vec3{.5f, -.9f, 0.f});

	scene_objects_.push_back(
		SceneObject{
			.mesh = move(mesh),
			.material = Material::FromType(MaterialType::Jade)
		});

	shader_program_.Enable();
	UpdateProjectionTransform();

	const auto& [eye, center, up] = camera_;
	view_transform = lookAt(eye, center, up);

	for (size_t i = 0; i < point_lights_.size(); ++i) {
		const auto& [position, color, attenuation] = point_lights_[i];
		shader_program_.SetUniform(format("point_lights[{}].position", i), vec3{view_transform * position});
		shader_program_.SetUniform(format("point_lights[{}].color", i), color);
		shader_program_.SetUniform(format("point_lights[{}].attenuation", i), attenuation);
	}
}

void Scene::Render(const float delta_time) {
	HandleContinuousInput(delta_time);
	UpdateProjectionTransform();

	for (const auto& [mesh, material] : scene_objects_) {

		// generally, normals should be transformed by the upper 3x3 inverse transpose of the view-model matrix, however,
		// this is unnecessary in this context because meshes are only transformed by rotations and translations (which are
		// orthogonal matrices and therefore the inverse transpose of the view-model matrix is to view-model matrix itself)
		// in addition to uniform scaling (which is undone when the transformed normal is renormalized in the vertex shader)
		const auto view_model_transform = view_transform * mesh.ModelTransform();
		shader_program_.SetUniform("view_model_transform", view_model_transform);
		shader_program_.SetUniform("normal_transform", mat3{view_model_transform});

		shader_program_.SetUniform("material.ambient", material.Ambient());
		shader_program_.SetUniform("material.diffuse", material.Diffuse());
		shader_program_.SetUniform("material.specular", material.Specular());
		shader_program_.SetUniform("material.shininess", material.Shininess() * 128.f);

		mesh.Render();
	}
}

void Scene::UpdateProjectionTransform() {

	if (const auto window_dimensions = window_.Size(); window_dimensions != prev_window_dimensions) {
		const auto [field_of_view_y, z_near, z_far] = view_frustum_;
		const auto [width, height] = window_dimensions;
		const auto aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
		const auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
		shader_program_.SetUniform("projection_transform", projection_transform);
		prev_window_dimensions = window_dimensions;
	}
}

void Scene::HandleDiscreteKeyPress(const int32_t key_code) {

	const auto scene_objects_size = static_cast<int32_t>(scene_objects_.size());
	if (active_scene_object_ >= scene_objects_size) return;

	switch (key_code) {
		case GLFW_KEY_S: {
			auto& mesh = scene_objects_[active_scene_object_].mesh;
			mesh = mesh::Simplify(mesh, .5f);
			break;
		}
		case GLFW_KEY_P:
			use_phong_shading = !use_phong_shading;
			shader_program_.SetUniform("use_phong_shading", use_phong_shading);
			break;
		case GLFW_KEY_N:
			if (++active_scene_object_ >= scene_objects_size) {
				active_scene_object_ = 0;
			}
			break;
		case GLFW_KEY_B:
			if (--active_scene_object_ < 0) {
				active_scene_object_ = std::min<>(0, scene_objects_size - 1);
			}
			break;
		default:
			break;
	}
}

void Scene::HandleContinuousInput(const float delta_time) {

	if (active_scene_object_ >= static_cast<int32_t>(scene_objects_.size())) return;

	static optional<dvec2> prev_cursor_position{};
	const auto translate_step = 1.25f * delta_time;
	const auto scale_step = .75f * delta_time;
	auto& mesh = scene_objects_[active_scene_object_].mesh;

	if (window_.IsKeyPressed(GLFW_KEY_W)) {
		mesh.Translate(vec3{0.f, translate_step, 0.f});
	} else if (window_.IsKeyPressed(GLFW_KEY_X)) {
		mesh.Translate(vec3{0.f, -translate_step, 0.f});
	}

	if (window_.IsKeyPressed(GLFW_KEY_A)) {
		mesh.Translate(vec3{-translate_step, 0.f, 0.f});
	} else if (window_.IsKeyPressed(GLFW_KEY_D)) {
		mesh.Translate(vec3{translate_step, 0.f, 0.f});
	}

	if (window_.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && window_.IsKeyPressed(GLFW_KEY_EQUAL)) {
		mesh.Scale(vec3{1.f + scale_step});
	} else if (window_.IsKeyPressed(GLFW_KEY_MINUS)) {
		mesh.Scale(vec3{1.f - scale_step});
	}

	if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
		const auto cursor_position = window_.GetCursorPosition();
		if (prev_cursor_position) {
			if (const auto axis_and_angle = arcball::GetRotation(*prev_cursor_position, cursor_position, window_.Size())) {
				const auto& [view_rotation_axis, angle] = *axis_and_angle;
				const auto view_model_transform = view_transform * mesh.ModelTransform();
				const auto model_rotation_axis = mat3{transpose(view_model_transform)} * view_rotation_axis;
				mesh.Rotate(normalize(model_rotation_axis), angle);
			}
		}
		prev_cursor_position = cursor_position;
	} else if (prev_cursor_position.has_value()) {
		prev_cursor_position = nullopt;
	}
}
