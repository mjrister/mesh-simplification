#include <algorithm>
#include <iostream>
#include <optional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SILENT_WARNINGS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "engine/graphics/mesh.h"
#include "engine/graphics/obj_loader.h"
#include "engine/graphics/shader_program.h"
#include "engine/graphics/texture2d.h"
#include "engine/window.h"

namespace {
	std::optional<glm::dvec2> prev_cursor_position;

	glm::vec2 GetNormalizedCursorPosition(
		const glm::dvec2& cursor_position, const std::int32_t window_width, const std::int32_t window_height) {
		const auto x = static_cast<GLfloat>(cursor_position.x * 2.0 / window_width - 1.0);
		const auto y = static_cast<GLfloat>(cursor_position.y * 2.0 / window_height - 1.0);
		return {std::clamp(x, -1.0f, 1.0f), std::clamp(-y, -1.0f, 1.0f)};
	}

	glm::vec3 GetArcBallPosition(
		const glm::dvec2 cursor_position, const std::int32_t window_width, const std::int32_t window_height) {
		const auto cursor_position_norm = GetNormalizedCursorPosition(cursor_position, window_width, window_height);
		const auto x = cursor_position_norm.x;
		const auto y = cursor_position_norm.y;
		if (const auto z = x * x + y * y; z <= 1.0f) {
			return glm::vec3{x, y, std::sqrt(1.0f - z)};
		}
		return glm::normalize(glm::vec3{x, y, 0.0f});
	}

	void HandleInput(const gfx::Window& window, gfx::Mesh& mesh, const glm::mat4& view_model_transform) {
		static constexpr GLfloat translate_step{0.01f};
		static constexpr GLfloat scale_step{0.01f};

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

			if (prev_cursor_position.has_value() && *prev_cursor_position != cursor_position) {
				const auto [width, height] = window.Size();
				const auto a = GetArcBallPosition(*prev_cursor_position, width, height);
				const auto b = GetArcBallPosition(cursor_position, width, height);

				if (const auto angle = std::acos(std::min<>(1.0f, glm::dot(a, b))); angle > 1e-3f) {
					const auto view_rotation_axis = glm::cross(a, b);
					const auto view_model_inverse_transform = glm::inverse(view_model_transform);
					const auto model_rotation_axis = glm::mat3{view_model_inverse_transform} * view_rotation_axis;
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
		gfx::Window window{"OpenGL", window_width, window_height, opengl_major_version, opengl_minor_version};

		gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const gfx::Texture2d texture2d{"resources/textures/bob.png"};
		texture2d.Bind();

		auto mesh = gfx::obj_loader::LoadMesh("resources/models/bob.obj");
		mesh.Scale(glm::vec3{0.5f});
		mesh.Rotate(glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(45.0f));

		constexpr GLfloat field_of_view{glm::radians(45.0f)}, z_near{0.1f}, z_far{100.0f};
		constexpr auto aspect_ratio = static_cast<GLfloat>(window_width) / window_height;
		auto projection_transform = glm::perspective(field_of_view, aspect_ratio, z_near, z_far);
		shader_program.SetUniform("projection_transform", projection_transform);

		constexpr glm::vec3 eye{0.0f, 0.0f, 2.0f}, center{0.0f}, up{0.0f, 1.0f, 0.0f};
		const auto view_transform = glm::lookAt(eye, center, up);

		constexpr glm::vec3 ambient_color{0.3f};
		shader_program.SetUniform("ambient_color", ambient_color);

		constexpr glm::vec3 position{-1.0f, 1.0f, 1.0f};
		constexpr glm::vec3 color{1.0f};
		constexpr GLfloat intensity{1.0f};
		constexpr glm::vec3 attenuation{0.0f, 1.0f, 0.0f};
		shader_program.SetUniform("point_light.position", glm::mat3{view_transform} * position);
		shader_program.SetUniform("point_light.color", color);
		shader_program.SetUniform("point_light.intensity", intensity);
		shader_program.SetUniform("point_light.attenuation", attenuation);

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (const auto [width, height] = window.Size(); width != window_width || height != window_height) {
				projection_transform = glm::perspective(field_of_view, window.AspectRatio(), z_near, z_far);
				shader_program.SetUniform("projection_transform", projection_transform);
			}

			const auto model_view_transform = view_transform * mesh.Model();
			shader_program.SetUniform("view_model_transform", model_view_transform);

			const auto normal_matrix = glm::inverse(transpose(glm::mat3{model_view_transform}));
			shader_program.SetUniform("normal_transform", normal_matrix);

			HandleInput(window, mesh, model_view_transform);
			mesh.Render();
			window.Update();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
