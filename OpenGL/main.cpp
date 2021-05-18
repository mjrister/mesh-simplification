#include <algorithm>
#include <iostream>
#include <optional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/mesh.h"
#include "graphics/obj_loader.h"
#include "graphics/shader_program.h"
#include "graphics/texture2d.h"
#include "graphics/window.h"

namespace {
	std::optional<glm::vec2> prev_cursor_position;

	glm::vec3 GetArcBallPosition(
		const glm::vec2 cursor_position, const std::int32_t width, const std::int32_t height) {
		constexpr auto min = -1.0f, max = 1.0f;
		const auto x = std::clamp(cursor_position.x * 2.0f / static_cast<GLfloat>(width) - 1.0f, min, max);
		const auto y = -std::clamp(cursor_position.y * 2.0f / static_cast<GLfloat>(height) - 1.0f, min, max);

		if (const auto c = x * x + y * y; c <= 1.0f) {
			return glm::vec3{x, y, std::sqrt(1.0f - c)};
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

			if (!prev_cursor_position.has_value()) {
				prev_cursor_position = cursor_position;
			}

			if (static constexpr auto epsilon = std::numeric_limits<GLfloat>::epsilon();
				glm::all(glm::epsilonNotEqual(*prev_cursor_position, cursor_position, epsilon))) {

				const auto [width, height] = window.Dimensions();
				const auto a = GetArcBallPosition(*prev_cursor_position, width, height);
				const auto b = GetArcBallPosition(cursor_position, width, height);

				if (const auto angle = std::acos(std::min<>(1.0f, glm::dot(a, b))); std::abs(angle) > epsilon) {
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
		mesh.Initialize();
		mesh.Scale(glm::vec3{0.5f});

		constexpr auto aspect_ratio = static_cast<GLfloat>(window_width) / window_height;
		const auto projection_transform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
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
