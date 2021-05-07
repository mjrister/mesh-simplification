#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "graphics/mesh.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/window.hpp"

Mesh CreateTriangle() {

	const std::vector vertices{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	const std::vector indices{
		0u, 1u, 2u
	};

	return Mesh{vertices, indices};
}

int main() {

	try {
		const Window window{"OpenGL", 640, 480};
		const ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const auto triangle = CreateTriangle();

		while (!window.Closed()) {
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			triangle.Render();
			window.SwapBuffers();
			glfwPollEvents();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
