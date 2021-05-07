#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "graphics/mesh.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/texture2d.hpp"
#include "graphics/window.hpp"

Mesh CreateContainer() {

	const std::vector<GLfloat> vertices{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	const std::vector<GLfloat> texture_coordinates{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	const std::vector<GLuint> indices{
		0u, 1u, 2u,
		0u, 2u, 3u
	};

	return Mesh{vertices, texture_coordinates, indices};
}

int main() {

	try {
		const Window window{"OpenGL", 640, 480};
		const ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		const Texture2d texture2d{"resources/textures/container.jpg"};

		shader_program.Enable();
		texture2d.Bind();

		const auto container = CreateContainer();

		while (!window.Closed()) {
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
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
