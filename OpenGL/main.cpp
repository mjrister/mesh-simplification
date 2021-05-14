#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/mesh.hpp"
#include "graphics/object_loader.hpp"
#include "graphics/point_light.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/texture2d.h"
#include "graphics/window.hpp"

void InitializeLights(gfx::ShaderProgram& shader_program, const glm::mat4 view_matrix) {

	constexpr AmbientLight ambient_light{
		.color = glm::vec3{0.3f},
		.intensity = 1.0f
	};

	shader_program.SetUniform("ambient_light.color", ambient_light.color);
	shader_program.SetUniform("ambient_light.intensity", ambient_light.intensity);

	constexpr PointLight point_light{
		.position = glm::vec3{0.0f, 1.0f, 0.0f},
		.color = glm::vec3{1.0f},
		.intensity = 1.0f,
		.attenuation = {
			.constant = 0.0f,
			.linear = 0.0f,
			.exponent = 1.0f
		}
	};

	shader_program.SetUniform("point_light.position", glm::mat3{view_matrix} * point_light.position);
	shader_program.SetUniform("point_light.color", point_light.color);
	shader_program.SetUniform("point_light.intensity", point_light.intensity);
	shader_program.SetUniform("point_light.attenuation.constant", point_light.attenuation.constant);
	shader_program.SetUniform("point_light.attenuation.linear", point_light.attenuation.linear);
	shader_program.SetUniform("point_light.attenuation.exponent", point_light.attenuation.exponent);
}

int main() {

	try {
		constexpr std::int32_t width{1280}, height{960};
		constexpr std::int32_t major_version{4}, minor_version{6};
		const gfx::Window window{"OpenGL", std::make_pair(width, height), std::make_pair(major_version, minor_version)};

		gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const gfx::Texture2d texture2d{"resources/textures/bob.png", GL_TEXTURE0};
		texture2d.Bind();

		auto mesh = gfx::ObjectLoader::LoadMesh("resources/models/bob.obj");
		mesh.Initialize();

		constexpr auto aspect_ratio = static_cast<GLfloat>(width) / height;
		const auto projection_transform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
		shader_program.SetUniform("projection_transform", projection_transform);

		constexpr glm::vec3 eye{0.0f, 0.0f, 3.0f}, center{0.0f}, up{0.0f, 1.0f, 0.0f};
		const auto view_transform = glm::lookAt(eye, center, up);

		InitializeLights(shader_program, view_transform);

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			const auto time = static_cast<GLfloat>(glfwGetTime());
			const auto model_transform = glm::rotate(glm::mat4{1.0f}, time, glm::vec3{0.0f, 1.0f, 0.0f});
			const auto model_view_transform = view_transform * model_transform;
			shader_program.SetUniform("model_view_transform", model_view_transform);

			const auto normal_transform = glm::inverse(transpose(glm::mat3{model_view_transform}));
			shader_program.SetUniform("normal_transform", normal_transform);

			mesh.Render();

			window.SwapBuffers();
			glfwPollEvents();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
