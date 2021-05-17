#pragma once

#include <string_view>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace gfx {
	class Mesh;

	class Window {

	public:
		Window(
			std::string_view title,
			std::int32_t width,
			std::int32_t height,
			std::int32_t opengl_major_version,
			std::int32_t opengl_minor_version);
		~Window() { glfwTerminate(); }

		Window(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = delete;

		[[nodiscard]] bool Closed() const noexcept {
			return glfwWindowShouldClose(window_);
		}

		[[nodiscard]] bool IsKeyPressed(const std::int32_t key) const noexcept {
			return glfwGetKey(window_, key) == GLFW_PRESS;
		}

		void Update() const noexcept {
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}

	private:
		GLFWwindow* window_{};
		glm::vec2 prev_cursor_position_{-1.0f}, cursor_position_{};
	};
}
