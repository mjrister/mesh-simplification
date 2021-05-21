#include "window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
	void HandleDebugMessageReceived(
		const GLenum source,
		const GLenum type,
		const GLuint /*id*/,
		const GLenum severity,
		const GLsizei /*length*/,
		const GLchar* const message,
		const void* /*user_param*/) {

		std::cout << "[OpenGL Debug] Source: ";
		switch (source) {
			case GL_DEBUG_SOURCE_API: std::cout << "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "WINDOW SYSTEM"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "SHADER COMPILER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "THIRD PARTY"; break;
			case GL_DEBUG_SOURCE_APPLICATION: std::cout << "APPLICATION"; break;
			default: std::cout << "OTHER"; break;
		} std::cout << ", ";

		std::cout << "Type: ";
		switch (type) {
			case GL_DEBUG_TYPE_ERROR: std::cout << "ERROR"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY"; break;
			case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE"; break;
			default: std::cout << "OTHER"; break;
		} std::cout << ", ";

		std::cout << "Severity: ";
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH: std::cout <<  "HIGH"; break;
			case GL_DEBUG_SEVERITY_MEDIUM: std::cout <<  "MEDIUM"; break;
			case GL_DEBUG_SEVERITY_LOW: std::cout <<  "LOW"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout <<  "NOTIFICATION"; break;
			default: std::cout <<  "OTHER"; break;
		} std::cout << std::endl;

		std::cout << message << std::endl;
	}

	void InitializeGlfw(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwSwapInterval(1);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwSetErrorCallback([](const std::int32_t error_code, const char* const description) {
			std::cerr << "GLFW Error (" << error_code << "): " << description << std::endl;
		});
#endif
	}

	void InitializeGl3w(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

		if (gl3wInit()) throw std::runtime_error{"OpenGL initialization failed"};

		if (!gl3wIsSupported(opengl_major_version, opengl_minor_version)) {
			std::ostringstream oss;
			oss << "OpenGL " << opengl_major_version << "." << opengl_minor_version << " not supported";
			throw std::runtime_error{oss.str()};
		}

#if _DEBUG
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << ", "
			<< "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(HandleDebugMessageReceived, nullptr);
#endif
	}
}

Window::Window(
	const std::string_view title,
	const std::int32_t width,
	const std::int32_t height,
	const std::int32_t opengl_major_version,
	const std::int32_t opengl_minor_version) {

	InitializeGlfw(opengl_major_version, opengl_minor_version);

	window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!window_) throw std::runtime_error{"Window creation failed"};

	glfwMakeContextCurrent(window_);
	glfwSetFramebufferSizeCallback(
		window_, [](GLFWwindow* const /*window*/, const std::int32_t width, const std::int32_t height) noexcept {
			glViewport(0, 0, width, height);
		});
	glfwSetKeyCallback(
		window_,
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
	glEnable(GL_CULL_FACE);
}