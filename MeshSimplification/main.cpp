#include <cmath>
#include <optional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/arcball.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/obj_loader.h"
#include "graphics/shader_program.h"
#include "graphics/window.h"

using namespace geometry;
using namespace gfx;
using namespace glm;
using namespace std;

namespace {
	void HandleInput(
		const Window& window, const float delta_time, const mat4 view_model_transform, Mesh& mesh) {
		static optional<dvec2> prev_cursor_position{};
		const auto translate_step = 1.25f * delta_time;
		const auto scale_step = .75f * delta_time;

		if (window.IsKeyPressed(GLFW_KEY_W)) {
			const vec3 translate{0.f, translate_step, 0.f};
			mesh.Translate(translate);
		} else if (window.IsKeyPressed(GLFW_KEY_S)) {
			const vec3 translate{0.f, -translate_step, 0.f};
			mesh.Translate(translate);
		}

		if (window.IsKeyPressed(GLFW_KEY_A)) {
			const vec3 translate{-translate_step, 0.f, 0.f};
			mesh.Translate(translate);
		} else if (window.IsKeyPressed(GLFW_KEY_D)) {
			const vec3 translate{translate_step, 0.f, 0.f};
			mesh.Translate(translate);
		}

		if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && window.IsKeyPressed(GLFW_KEY_EQUAL)) {
			const vec3 scale{1.f + scale_step};
			mesh.Scale(scale);
		} else if (window.IsKeyPressed(GLFW_KEY_MINUS)) {
			const vec3 scale{1.f - scale_step};
			mesh.Scale(scale);
		}

		if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
			const auto cursor_position = window.GetCursorPosition();
			if (prev_cursor_position) {
				if (const auto axis_and_angle = arcball::GetRotation(*prev_cursor_position, cursor_position, window.Size())) {
					const auto& [view_rotation_axis, angle] = *axis_and_angle;
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

int main() {

	try {
		int32_t window_width = 1280, window_height = 960;
		const auto window_dimensions = make_pair(window_width, window_height);
		constexpr auto opengl_version = make_pair(4, 6);
		Window window{"Mesh Simplification", window_dimensions, opengl_version};

		auto mesh = obj_loader::LoadMesh("models/bunny.obj");
		mesh.Scale(vec3{.25f});
		mesh.Translate({vec3{.5f, -.9f, 0.f}});

		ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		window.OnKeyPress([&](const auto& key) {
			if (key == GLFW_KEY_S) {
				mesh = mesh::Simplify(mesh, .5f);
			}
		});

		constexpr float field_of_view_y{radians(45.f)}, z_near{.1f}, z_far{100.f};
		auto aspect_ratio = static_cast<float>(window_width) / window_height;
		auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
		shader_program.SetUniform("projection_transform", projection_transform);

		constexpr vec3 eye{0.f, 0.f, 2.f}, center{0.f}, up{0.f, 1.f, 0.f};
		const auto view_transform = lookAt(eye, center, up);

		float point_light_angle = 0.f;
		shader_program.SetUniform("point_light.color", vec3{1.f});
		shader_program.SetUniform("point_light.attenuation", vec3{0.f, 0.f, 1.f});

		constexpr auto material = Material::Jade();
		shader_program.SetUniform("material.ambient", material.Ambient());
		shader_program.SetUniform("material.diffuse", material.Diffuse());
		shader_program.SetUniform("material.specular", material.Specular());
		shader_program.SetUniform("material.shininess", material.Shininess() * 128.f);

		for (double previous_time = glfwGetTime(); !window.Closed();) {
			const double current_time = glfwGetTime();
			const auto delta_time = static_cast<float>(current_time - previous_time);
			previous_time = current_time;

			if (const auto [width, height] = window.Size(); width != window_width || height != window_height) {
				window_width = width;
				window_height = height;
				aspect_ratio = static_cast<float>(window_width) / window_height;
				projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
				shader_program.SetUniform("projection_transform", projection_transform);
			}

			const auto view_model_transform = view_transform * mesh.ModelTransform();
			shader_program.SetUniform("view_model_transform", view_model_transform);
			shader_program.SetUniform("normal_transform", inverse(transpose(view_model_transform)));

			point_light_angle += .5f * delta_time;
			const vec4 point_light_position{cos(point_light_angle), sin(point_light_angle), 1.5f, 1.f};
			shader_program.SetUniform("point_light.position", vec3{view_transform * point_light_position});

			HandleInput(window, delta_time, view_model_transform, mesh);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			mesh.Render();
			window.Update();
		}
	} catch (const exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
