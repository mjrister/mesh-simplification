#include "window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
	void HandleDebugMessageReceived(
		const GLenum source,
		const GLenum type,
		const GLuint id,
		const GLenum severity,
		const GLsizei /*length*/,
		const GLchar* const message,
		const void* /*user_param*/) {

		std::string message_source;
		switch (source) {
			case GL_DEBUG_SOURCE_API: message_source = "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: message_source = "WINDOW SYSTEM"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: message_source = "SHADER COMPILER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: message_source = "THIRD PARTY"; break;
			case GL_DEBUG_SOURCE_APPLICATION: message_source = "APPLICATION"; break;
			default: message_source = "OTHER"; break;
		}

		std::string message_type;
		switch (type) {
			case GL_DEBUG_TYPE_ERROR: message_type = "ERROR"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: message_type = "DEPRECATED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: message_type = "UNDEFINED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_PORTABILITY: message_type = "PORTABILITY"; break;
			case GL_DEBUG_TYPE_PERFORMANCE: message_type = "PERFORMANCE"; break;
			default: message_type = "OTHER"; break;
		}

		std::string message_severity;
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH: message_severity = "HIGH"; break;
			case GL_DEBUG_SEVERITY_MEDIUM: message_severity = "MEDIUM"; break;
			case GL_DEBUG_SEVERITY_LOW: message_severity = "LOW"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: message_severity = "NOTIFICATION"; break;
			default: message_severity = "OTHER"; break;
		}

		std::cout << "OpenGL Debug (" << id << "): "
			<< "Source: " << message_source << ", "
			<< "Type: " << message_type << ", "
			<< "Severity: " << message_severity << std::endl
			<< message << std::endl;
	}

	void InitializeGlfw(const std::pair<const std::int32_t, const std::int32_t>& opengl_version) {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		const auto [major_version, minor_version] = opengl_version;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
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

	void InitializeGl3w(const std::pair<const std::int32_t, const std::int32_t>& opengl_version) {

		if (gl3wInit()) throw std::runtime_error{"OpenGL initialization failed"};

		const auto [major_version, minor_version] = opengl_version;
		if (!gl3wIsSupported(major_version, minor_version)) {
			std::ostringstream oss;
			oss << "OpenGL " << major_version << "." << minor_version << " not supported";
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
	const std::pair<const std::int32_t, const std::int32_t>& window_size,
	const std::pair<const std::int32_t, const std::int32_t>& opengl_version) {

	InitializeGlfw(opengl_version);

	const auto [width, height] = window_size;
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

	InitializeGl3w(opengl_version);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
}

Window::~Window() {
	if (window_) {
		glfwDestroyWindow(window_);
	}
	glfwTerminate();
}
