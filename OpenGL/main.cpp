#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

int main() {

	glfwSetErrorCallback([](const int error, const char* const description) {
		std::cerr << "Error " << error << ": " << description << std::endl;
	});

	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return EXIT_FAILURE;
	}

	constexpr auto major_version = 4;
	constexpr auto minor_version = 5;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);

	constexpr auto width = 640;
	constexpr auto height = 480;
	auto* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
	if (!window) {
		std::cerr << "Window creation failed" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	if (gl3wInit()) {
		std::cerr << "OpenGL initialization failed" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	if (!gl3wIsSupported(major_version, minor_version)) {
		std::cerr << "OpenGL " << major_version << "." << minor_version << "not supported";
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
