#include <algorithm>
#include <iostream>
#include <optional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/arcball.h"
#include "engine/window.h"

#include "engine/graphics/material.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/obj_loader.h"
#include "engine/graphics/shader_program.h"
#include "engine/graphics/texture2d.h"

namespace {
	void HandleInput(const Window& window, const glm::mat4 view_model_transform, gfx::Mesh& mesh) {
		static constexpr GLfloat translate_step{0.01f};
		static constexpr GLfloat scale_step{0.01f};
		static std::optional<glm::dvec2> prev_cursor_position{};

		if (window.IsKeyPressed(GLFW_KEY_W)) {
			static constexpr glm::vec3 translate{0.0f, translate_step, 0.0f};
			mesh.Translate(translate);
		} else if (window.IsKeyPressed(GLFW_KEY_X)) {
			static constexpr glm::vec3 translate{0.0f, -translate_step, 0.0f};
			mesh.Translate(translate);
		}

		if (window.IsKeyPressed(GLFW_KEY_A)) {
			static constexpr glm::vec3 translate{-translate_step, 0.0f, 0.0f};
			mesh.Translate(translate);
		} else if (window.IsKeyPressed(GLFW_KEY_D)) {
			static constexpr glm::vec3 translate{translate_step, 0.0f, 0.0f};
			mesh.Translate(translate);
		}

		if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && window.IsKeyPressed(GLFW_KEY_EQUAL)) {
			static constexpr glm::vec3 scale{1.0f + scale_step};
			mesh.Scale(scale);
		} else if (window.IsKeyPressed(GLFW_KEY_MINUS)) {
			static constexpr glm::vec3 scale{1.0f - scale_step};
			mesh.Scale(scale);
		}

		if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
			const auto cursor_position = window.GetCursorPosition();
			if (prev_cursor_position) {
				if (const auto axis_and_angle = arcball::GetRotation(window, cursor_position, *prev_cursor_position)) {
					const auto& [view_rotation_axis, angle] = *axis_and_angle;
					const auto view_model_transform_inv = glm::inverse(view_model_transform);
					const auto model_rotation_axis = glm::mat3{view_model_transform_inv} * view_rotation_axis;
					mesh.Rotate(model_rotation_axis, angle);
				}
			}
			prev_cursor_position = cursor_position;
		} else if (prev_cursor_position.has_value()) {
			prev_cursor_position = std::nullopt;
		}
	}
}

int main() {

	try {
		constexpr std::int32_t window_width{1280}, window_height{960};
		constexpr std::int32_t opengl_major_version{4}, opengl_minor_version{6};
		Window window{"OpenGL", window_width, window_height, opengl_major_version, opengl_minor_version};

		gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		auto mesh = gfx::obj_loader::LoadMesh("resources/models/bunny.obj");
		mesh.Scale(glm::vec3{0.25f});
		mesh.Translate(glm::vec3{0.25f, -0.75f, 0.0f});

		constexpr GLfloat field_of_view{glm::radians(45.0f)}, z_near{0.1f}, z_far{100.0f};
		constexpr auto original_aspect_ratio = static_cast<GLfloat>(window_width) / window_height;
		auto projection_transform = glm::perspective(field_of_view, original_aspect_ratio, z_near, z_far);
		shader_program.SetUniform("projection_transform", projection_transform);

		constexpr glm::vec3 eye{0.f, 0.0f, 2.0f}, center{0.0f}, up{0.0f, 1.0f, 0.0f};
		const auto view_transform = glm::lookAt(eye, center, up);

		constexpr glm::vec3 position{-1.0f, 1.0f, 1.0f};
		constexpr glm::vec3 color{1.0f};
		constexpr GLfloat intensity{1.0f};
		constexpr glm::vec3 attenuation{0.0f, 1.0f, 0.0f};
		shader_program.SetUniform("point_light.position", glm::mat3{view_transform} * position);
		shader_program.SetUniform("point_light.color", color);
		shader_program.SetUniform("point_light.intensity", intensity);
		shader_program.SetUniform("point_light.attenuation", attenuation);

		constexpr auto material = gfx::Material::Jade();
		shader_program.SetUniform("material.ambient", material.Ambient());
		shader_program.SetUniform("material.diffuse", material.Diffuse());
		shader_program.SetUniform("material.specular", material.Specular());
		shader_program.SetUniform("material.shininess", material.Shininess() * 128.0f);

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (const auto [width, height] = window.Size(); width != window_width || height != window_height) {
				const auto aspect_ratio = static_cast<GLfloat>(width) / height;
				projection_transform = glm::perspective(field_of_view, aspect_ratio, z_near, z_far);
				shader_program.SetUniform("projection_transform", projection_transform);
			}

			const auto view_model_transform = view_transform * mesh.Model();
			shader_program.SetUniform("view_model_transform", view_model_transform);

			const auto normal_transform = glm::inverse(transpose(glm::mat3{view_model_transform}));
			shader_program.SetUniform("normal_transform", normal_transform);

			HandleInput(window, view_model_transform, mesh);
			mesh.Render();
			window.Update();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
