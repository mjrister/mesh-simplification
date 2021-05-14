#pragma once

#include <string_view>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace gfx {

	class Window {

	public:
		Window(
			std::string_view title,
			const std::pair<const std::int32_t, const std::int32_t>& width_and_height,
			const std::pair<const std::int32_t, const std::int32_t>& opengl_major_and_minor_version);
		~Window() { glfwTerminate(); }

		Window(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = delete;

		[[nodiscard]] bool Closed() const noexcept {
			return glfwWindowShouldClose(window_);

		}

		void Update() const noexcept {
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}

	private:
		GLFWwindow* window_{};
	};
}
