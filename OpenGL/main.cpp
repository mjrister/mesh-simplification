#include <iostream>
#include <sstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

constexpr auto opengl_major_version = 4;
constexpr auto opengl_minor_version = 6;

void InitializeGlfw() {

	glfwSetErrorCallback([](const int error, const char* const description) {
		std::cerr << "Error " << error << ": " << description << std::endl;
	});

	if (!glfwInit()) {
		throw std::runtime_error{"GLFW initialization failed"};
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* CreateGlfwWindow(const char* const title, const int width, const int height) {

	if (auto* window = glfwCreateWindow(width, height, title, nullptr, nullptr)) {
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		glfwSetFramebufferSizeCallback(
			window, [](GLFWwindow* window, const int width, const int height) {
				glViewport(0, 0, width, height);
			});
		glfwSetKeyCallback(
			window, [](GLFWwindow* window, const int key, const int scancode, const int action, const int modifiers) {
				if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
					glfwSetWindowShouldClose(window, true);
				}
			});
		return window;
	}

	throw std::runtime_error{"Window creation failed"};
}

void InitializeGl3w() {

	if (gl3wInit()) {
		throw std::runtime_error{"OpenGL initialization failed"};
	}

	if (!gl3wIsSupported(opengl_major_version, opengl_minor_version)) {
		std::ostringstream oss;
		oss << "OpenGL " << opengl_major_version << "." << opengl_minor_version << "not supported";
		throw std::runtime_error{oss.str()};
	}
}

int main() {

	try {
		InitializeGlfw();
		auto* window = CreateGlfwWindow("OpenGL", 640, 480);
		InitializeGl3w();

		glClearColor(0.75f, 0.75f, 0.75f, 0.5f);
		glEnable(GL_DEPTH_TEST);

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
