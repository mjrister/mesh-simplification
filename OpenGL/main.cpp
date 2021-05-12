#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/mesh.hpp"
#include "graphics/object_loader.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/texture2d.hpp"
#include "graphics/window.hpp"

int main() {

	try {
		constexpr std::int32_t width{1280}, height{960};
		constexpr std::int32_t major_version{4}, minor_version{6};
		const gfx::Window window{"OpenGL", std::make_pair(width, height), std::make_pair(major_version, minor_version)};

		const gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const gfx::Texture2d texture2d{"resources/textures/spot.png", GL_TEXTURE0};
		texture2d.Bind();

		auto mesh = gfx::ObjectLoader::LoadMesh("resources/models/spot.obj");
		mesh.Initialize();

		const auto aspect_ratio = static_cast<GLfloat>(width) / height;
		const auto projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
		const auto view = glm::lookAt(glm::vec3{0.0f, 0.0f, 3.0f}, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			auto model = glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f});
			model = glm::rotate(model, static_cast<GLfloat>(glfwGetTime()) / 1.5f, glm::vec3{0.0f, 1.0f, 0.0f});
			shader_program.SetUniform("model_view_projection", projection * view * model);
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
