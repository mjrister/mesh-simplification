#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace gfx {

	class Window {

	public:
		Window(const std::string_view title,
		       const std::pair<std::int32_t, std::int32_t>& width_and_height,
		       const std::pair<std::int32_t, std::int32_t>& opengl_major_and_minor_version) {

			const auto [opengl_major_version, opengl_minor_version] = opengl_major_and_minor_version;
			InitializeGlfw(opengl_major_version, opengl_minor_version);

			const auto [width, height] = width_and_height;
			window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
			if (!window_) throw std::runtime_error{"Window creation failed"};

			glfwMakeContextCurrent(window_);
			glfwSetFramebufferSizeCallback(window_, OnWindowResize);
			glfwSetKeyCallback(window_, OnKeyPress);

			InitializeGl3w(opengl_major_version, opengl_minor_version);
			glEnable(GL_DEPTH_TEST);
		}

		Window(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = delete;

		~Window() {
			if (window_) glfwDestroyWindow(window_);
			glfwTerminate();
		}

		[[nodiscard]] bool Closed() const noexcept {
			return glfwWindowShouldClose(window_);
		}

		void SwapBuffers() const noexcept {
			glfwSwapBuffers(window_);
		}

	private:
		static void InitializeGlfw(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

			if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		static void InitializeGl3w(const std::int32_t opengl_major_version, const std::int32_t opengl_minor_version) {

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

		static void OnWindowResize(GLFWwindow* /*window*/, const int32_t width, const int32_t height) noexcept {
			glViewport(0, 0, width, height);
		}

		static void OnKeyPress(
			GLFWwindow* window,
			const int32_t key,
			const int32_t /*scancode*/,
			const int32_t action,
			const int32_t /*modifiers*/) noexcept {

			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, true);
			}
		}

		static void GLAPIENTRY PrintDebugErrorMessage(
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

		GLFWwindow* window_{};
	};
}
