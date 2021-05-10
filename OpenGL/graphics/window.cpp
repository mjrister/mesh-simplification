#include "window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

	void OnWindowResize(GLFWwindow* /*window*/, const int32_t width, const int32_t height) noexcept {
		glViewport(0, 0, width, height);
	}

	void OnKeyPress(
		GLFWwindow* window,
		const int32_t key,
		const int32_t /*scancode*/,
		const int32_t action,
		const int32_t /*modifiers*/) noexcept {

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	void GLAPIENTRY PrintDebugErrorMessage(
		const GLenum /*source*/,
		const GLenum type,
		const GLuint /*id*/,
		const GLenum severity,
		const GLsizei /*length*/,
		const GLchar* message,
		const void* /*user_param*/) {

		if (type == GL_DEBUG_TYPE_ERROR) {
			std::cerr << "GL ERROR: "
				<< "type: 0x" << type << ", "
				<< "severity: 0x" << severity << ", "
				<< "message: " << message << std::endl;
		}
	}

	void InitializeGlfw(const int32_t opengl_major_version, const int32_t opengl_minor_version) {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	void InitializeGl3w(const int32_t opengl_major_version, const int32_t opengl_minor_version) {

		if (gl3wInit()) throw std::runtime_error{"OpenGL initialization failed"};

		if (!gl3wIsSupported(opengl_major_version, opengl_minor_version)) {
			std::ostringstream oss;
			oss << "OpenGL " << opengl_major_version << "." << opengl_minor_version << "not supported";
			throw std::runtime_error{oss.str()};
		}

#if _DEBUG
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
			<< "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(PrintDebugErrorMessage, nullptr);
#endif
	}
}

gfx::Window::Window(
	const std::string_view title,
	const std::pair<int32_t, int32_t>& width_and_height,
	const std::pair<int32_t, int32_t>& opengl_major_and_minor_version) {

	const auto [major_version, minor_version] = opengl_major_and_minor_version;
	InitializeGlfw(major_version, minor_version);

	const auto [width, height] = width_and_height;
	window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!window_) throw std::runtime_error{"Window creation failed"};

	glfwMakeContextCurrent(window_);
	glfwSetFramebufferSizeCallback(window_, OnWindowResize);
	glfwSetKeyCallback(window_, OnKeyPress);

	InitializeGl3w(major_version, minor_version);
	glEnable(GL_DEPTH_TEST);
}

gfx::Window::~Window() {
	if (window_) glfwDestroyWindow(window_);
	glfwTerminate();
}

bool gfx::Window::Closed() const noexcept {
	return glfwWindowShouldClose(window_);
}

void gfx::Window::SwapBuffers() const noexcept {
	glfwSwapBuffers(window_);
}
