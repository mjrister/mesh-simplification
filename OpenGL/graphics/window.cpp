#include "window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "graphics/mesh.h"

namespace {
	void HandleDebugMessageReceived(
		const GLenum message_source,
		const GLenum message_type,
		const GLuint message_id,
		const GLenum message_severity,
		const GLsizei /*message_length*/,
		const GLchar* const message,
		const void* /*user_param*/) {

		std::string source;
		switch (message_source) {
			case GL_DEBUG_SOURCE_API: source = "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source = "WINDOW SYSTEM"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "SHADER COMPILER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: source = "THIRD PARTY"; break;
			case GL_DEBUG_SOURCE_APPLICATION: source = "APPLICATION"; break;
			default: source = "OTHER"; break;
		}

		std::string type;
		switch (message_type) {
			case GL_DEBUG_TYPE_ERROR: type = "ERROR"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type = "UNDEFINED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_PORTABILITY: type = "PORTABILITY"; break;
			case GL_DEBUG_TYPE_PERFORMANCE: type = "PERFORMANCE"; break;
			default: type = "OTHER"; break;
		}

		std::string severity;
		switch (message_severity) {
			case GL_DEBUG_SEVERITY_HIGH: severity = "HIGH"; break;
			case GL_DEBUG_SEVERITY_MEDIUM: severity = "MEDIUM"; break;
			case GL_DEBUG_SEVERITY_LOW: severity = "LOW"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
			default: severity = "OTHER"; break;
		}

		std::cout << "OpenGL Debug (" << message_id << "): "
			<< "Source: " << source << ", Type: " << type << ", Severity: " << severity << std::endl
			<< message << std::endl;
	}

	void InitializeGlfw(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwSwapInterval(1);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
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

gfx::Window::Window(
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
}

//void gfx::Window::HandleInput(Mesh& mesh) {
//	static constexpr GLfloat translate_step{0.01f};
//	static constexpr GLfloat scale_step{0.01f};
//	static constexpr GLfloat rotation_step{0.025f};
//
//	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
//		static constexpr glm::vec3 translate{0.0f, translate_step, 0.0f};
//		mesh.Translate(translate);
//	} else if (glfwGetKey(window_, GLFW_KEY_X) == GLFW_PRESS) {
//		static constexpr glm::vec3 translate{0.0f, -translate_step, 0.0f};
//		mesh.Translate(translate);
//	}
//
//	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
//		static constexpr glm::vec3 translate{-translate_step, 0.0f, 0.0f};
//		mesh.Translate(translate);
//	} else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
//		static constexpr glm::vec3 translate{translate_step, 0.0f, 0.0f};
//		mesh.Translate(translate);
//	}
//
//	if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window_, GLFW_KEY_EQUAL) == GLFW_PRESS) {
//		static constexpr glm::vec3 scale{1.0f + scale_step};
//		mesh.Scale(scale);
//	} else if (glfwGetKey(window_, GLFW_KEY_MINUS)) {
//		static constexpr glm::vec3 scale{1.0f - scale_step};
//		mesh.Scale(scale);
//	}
//
//	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//		double x, y;
//		glfwGetCursorPos(window_, &x, &y);
//		cursor_position_ = glm::vec2{x, y};
//
//		if (prev_cursor_position_ != glm::vec2{-1.0f}) {
//			if (const auto cursor_delta = cursor_position_ - prev_cursor_position_; glm::length(cursor_delta) > 0.0f) {
//				mesh.Rotate(glm::vec3{cursor_delta.y, cursor_delta.x, 0.0f}, rotation_step);
//			}
//		}
//
//		prev_cursor_position_ = cursor_position_;
//	}
//}
