#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/mesh.hpp"
#include "graphics/object_loader.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/texture2d.hpp"
#include "graphics/window.hpp"

int main() {

	try {
		constexpr auto width = 1280, height = 960;
		const gfx::Window window{"OpenGL", width, height};

		const gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const gfx::Texture2d texture2d{"resources/textures/blub.png", GL_TEXTURE0};
		texture2d.Bind();

		auto container = gfx::ObjectLoader::LoadMesh("resources/models/blub.obj");
		container.Initialize();

		const auto projection = glm::perspective(glm::radians(45.0f), static_cast<GLfloat>(width) / height, 0.1f, 100.0f);
		const auto view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -3.0f});
		const auto projection_view = projection * view;

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			auto model = glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f});
			model = glm::rotate(model, static_cast<GLfloat>(glfwGetTime()) / 1.5f, glm::vec3{0.0f, 1.0f, 0.0f});
			shader_program.SetUniform("model_view_projection", projection_view * model);
			container.Render();
			window.SwapBuffers();
			glfwPollEvents();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
