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
#include "graphics/texture2d.hpp"
#include "graphics/window.hpp"

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
		const auto projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
		shader_program.SetUniform("projection_matrix", projection);

		const auto view = glm::lookAt(glm::vec3{0.0f, 0.0f, 3.0f}, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
		const auto scale = glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f});

		constexpr PointLight point_light{
			.position = glm::vec3{0.0, 1.0f, 0.0f},
			.color = glm::vec3{1.0f, 1.0f, 1.0f},
			.intensity = 1.0f,
			.attenuation = {
				.constant = 0.0f,
				.linear = 0.0f,
				.exponent = 1.0f
			}
		};

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			const auto model = glm::rotate(scale, static_cast<GLfloat>(glfwGetTime()), glm::vec3{0.0f, 1.0f, 0.0f});
			const auto model_view = view * model;
			shader_program.SetUniform("model_view_matrix", model_view);

			const auto normal_matrix = glm::inverse(glm::transpose(glm::mat3{model_view}));
			shader_program.SetUniform("normal_matrix", normal_matrix);

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
