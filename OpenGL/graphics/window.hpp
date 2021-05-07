#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Window {

public:
	Window(const std::string_view title, const int32_t width, const int32_t height) {

		InitializeGlfw();

		window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
		if (!window_) throw std::runtime_error{"Window creation failed"};

		glfwMakeContextCurrent(window_);
		glfwSetFramebufferSizeCallback(window_, OnWindowResize);
		glfwSetKeyCallback(window_, OnKeyPress);

		InitializeGl3w();
		glEnable(GL_DEPTH_TEST);
	}

	Window(const Window&) = delete;
	Window(Window&&) noexcept = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) noexcept = delete;

	~Window() {
		if (window_) {
			glfwDestroyWindow(window_);
		}
		glfwTerminate();
	}

	[[nodiscard]] bool Closed() const noexcept {
		return glfwWindowShouldClose(window_);
	}

	void SwapBuffers() const noexcept {
		glfwSwapBuffers(window_);
	}

private:
	static void InitializeGlfw() {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version_);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version_);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	static void InitializeGl3w() {

		if (gl3wInit()) throw std::runtime_error{"OpenGL initialization failed"};

		if (!gl3wIsSupported(opengl_major_version_, opengl_minor_version_)) {
			std::ostringstream oss;
			oss << "OpenGL " << opengl_major_version_ << "." << opengl_minor_version_ << "not supported";
			throw std::runtime_error{oss.str()};
		}

#if _DEBUG
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
			<< "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		GLint max_vertex_attributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
		std::cout << "Maximum vertex attributes supported: " << max_vertex_attributes << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(PrintDebugErrorMessage, nullptr);
#endif
	}

	static void OnWindowResize(GLFWwindow*, const int32_t width, const int32_t height) noexcept {
		glViewport(0, 0, width, height);
	}

	static void OnKeyPress(
		GLFWwindow* window, const int32_t key, const int32_t, const int32_t action, const int32_t) noexcept {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	static void GLAPIENTRY PrintDebugErrorMessage(
		const GLenum source,
		const GLenum type,
		const GLuint id,
		const GLenum severity,
		const GLsizei length,
		const GLchar* message,
		const void* user_param) {

		if (type == GL_DEBUG_TYPE_ERROR) {
			std::cerr << "GL ERROR: "
			<< "type: 0x" << type << ", "
			<< "severity: 0x" << severity << ", "
			<< "message: " << message << std::endl;
		}
	}

	static constexpr int32_t opengl_major_version_{4}, opengl_minor_version_{6};
	GLFWwindow* window_{nullptr};
};
