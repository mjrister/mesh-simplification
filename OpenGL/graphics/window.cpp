#include "window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

	void InitializeGlfw(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
	}

	void InitializeGl3w(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

		if (gl3wInit()) throw std::runtime_error{"OpenGL initialization failed"};

		if (!gl3wIsSupported(opengl_major_version, opengl_minor_version)) {
			std::ostringstream oss;
			oss << "OpenGL " << opengl_major_version << "." << opengl_minor_version << "not supported";
			throw std::runtime_error{oss.str()};
		}

#if _DEBUG
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << ", "
			<< "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
	}
}

gfx::Window::Window(
	const std::string_view title,
	const std::pair<const std::int32_t, const std::int32_t>& width_and_height,
	const std::pair<const std::int32_t, const std::int32_t>& opengl_major_and_minor_version) {

	const auto [opengl_major_version, opengl_minor_version] = opengl_major_and_minor_version;
	InitializeGlfw(opengl_major_version, opengl_minor_version);

	const auto [width, height] = width_and_height;
	window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!window_) throw std::runtime_error{"Window creation failed"};

	glfwMakeContextCurrent(window_);
	glfwSetFramebufferSizeCallback(window_,
		[](GLFWwindow* const /*window*/, const std::int32_t width, const std::int32_t height) noexcept {
			glViewport(0, 0, width, height);
		});
	glfwSetKeyCallback(window_,
		[](GLFWwindow* const window,
		   const std::int32_t key,
		   const std::int32_t /*scancode*/,
		   const std::int32_t action,
		   const std::int32_t /*modifiers*/) noexcept {

			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, true);
			}
		});

	InitializeGl3w(opengl_major_version, opengl_minor_version);
	glEnable(GL_DEPTH_TEST);
}

bool gfx::Window::Closed() const noexcept {
	return glfwWindowShouldClose(window_);
}

void gfx::Window::Update() const noexcept {
	glfwSwapBuffers(window_);
	glfwPollEvents();
}
