#include <iostream>
#include <sstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "window.h"

int main() {

	try {
		const Window window{"OpenGL", 640, 480};
		while (!window.Closed()) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			window.SwapBuffers();
			glfwPollEvents();
		}
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
