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
		constexpr auto width = 640, height = 480;
		const gfx::Window window{"OpenGL", width, height};
		const gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		const gfx::Texture2d texture2d{"resources/textures/bob.png", GL_TEXTURE0};

		shader_program.Enable();
		texture2d.Bind();

		auto container = gfx::ObjectLoader::LoadMesh("resources/models/bob.obj");
		container.Initialize();

		const auto projection = glm::perspective(glm::radians(45.0f), static_cast<GLfloat>(width) / height, 0.1f, 100.0f);
		const auto view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -3.0f});

		while (!window.Closed()) {
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			const auto model = glm::rotate(
				glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f}),
				static_cast<GLfloat>(glfwGetTime()),
				glm::vec3{0.0f, 1.0f, 0.0f});
			shader_program.SetUniform("model_view_projection", projection * view * model);
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
