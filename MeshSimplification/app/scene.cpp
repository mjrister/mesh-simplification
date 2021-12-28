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

Scene::Scene(Window& window, ShaderProgram& shader_program)
	: window_{window},
	  shader_program_{shader_program},
	  view_frustum_{
		  ViewFrustum{
			  .field_of_view_y = glm::radians(45.f),
			  .z_near = .1f,
			  .z_far = 100.f
		  }
	  },
	  camera_{
		  Camera{
			  .eye = vec3{0., 0., 2.f},
			  .center = vec3{0.f},
			  .up = vec3{0.f, 1.f, 0.f}
		  }
	  },
	  point_lights_{
		  {
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
		  }
	  } {

	auto mesh = obj_loader::LoadMesh("models/bunny.obj");
	mesh.Scale(vec3{.25f});
	mesh.Translate(vec3{.5f, -.9f, 0.f});

	scene_objects_.push_back(
		SceneObject{
			.mesh = move(mesh),
			.material = Material::FromType(MaterialType::Jade)
		});

	window.OnKeyPress([this](const auto key) { OnKeyPress(key); });
}

void Scene::Render(const float delta_time) {

	HandleInput(delta_time);
	shader_program_.Enable();

	const auto [field_of_view_y, z_near, z_far] = view_frustum_;
	const auto projection_transform = glm::perspective(field_of_view_y, window_.AspectRatio(), z_near, z_far);
	shader_program_.SetUniform("projection_transform", projection_transform);

	const auto& [eye, center, up] = camera_;
	const auto view_transform = glm::lookAt(eye, center, up);
	shader_program_.SetUniform("view_transform", view_transform);

	for (size_t i = 0; i < point_lights_.size(); ++i) {
		const auto& [position, color, attenuation] = point_lights_[i];
		shader_program_.SetUniform(format("point_lights[{}].position", i), vec3{view_transform * position});
		shader_program_.SetUniform(format("point_lights[{}].color", i), color);
		shader_program_.SetUniform(format("point_lights[{}].attenuation", i), attenuation);
	}

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

void Scene::OnKeyPress(const int32_t key_code) {

	const auto scene_objects_size = static_cast<int32_t>(scene_objects_.size());
	if (active_scene_object_ >= scene_objects_size) return;

	switch (key_code) {
		case GLFW_KEY_S: {
			auto& mesh = scene_objects_[active_scene_object_].mesh;
			mesh = mesh::Simplify(mesh, .5f);
			break;
		}
		case GLFW_KEY_P: {
			static auto use_phong_shading = false;
			use_phong_shading = !use_phong_shading;
			shader_program_.SetUniform("use_phong_shading", use_phong_shading);
		}
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

void Scene::HandleInput(const float delta_time) {

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
				const auto view_transform = glm::lookAt(camera_.eye, camera_.center, camera_.up);
				const auto view_model_transform = view_transform * mesh.ModelTransform();
				const auto model_rotation_axis = mat3{glm::inverse(view_model_transform)} * view_rotation_axis;
				mesh.Rotate(normalize(model_rotation_axis), angle);
			}
		}
		prev_cursor_position = cursor_position;
	} else if (prev_cursor_position.has_value()) {
		prev_cursor_position = nullopt;
	}
}
