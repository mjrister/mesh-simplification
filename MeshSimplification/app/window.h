#pragma once

#include <functional>
#include <string_view>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace app {

/** \brief An abstraction for a GLFW window. */
class Window {

public:
	/**
	 * \brief Initializes a window.
	 * \param title The window title.
	 * \param window_dimensions The window width and height.
	 * \param opengl_version The OpenGL major and minor version.
	 */
	Window(
		std::string_view title,
		const std::pair<const int, const int>& window_dimensions,
		const std::pair<const int, const int>& opengl_version);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) noexcept = delete;
	Window& operator=(Window&&) noexcept = delete;

	[[nodiscard]] const std::function<void(int)>& on_key_press() const noexcept { return on_key_press_; }
	void set_on_key_press(const std::function<void(int)>& on_key_press) { on_key_press_ = on_key_press; }

	/**
	 * \brief Gets the window size.
	 * \return A pair representing the window width and height.
	 */
	[[nodiscard]] std::pair<int, int> GetSize() const noexcept {
		int width, height;
		glfwGetWindowSize(window_, &width, &height);
		return {width, height};
	}

	/**
	 * \brief Gets the cursor position.
	 * \return The (x,y) coordinates of the cursor in the window.
	 */
	[[nodiscard]] glm::dvec2 GetCursorPosition() const noexcept {
		double x, y;
		glfwGetCursorPos(window_, &x, &y);
		return {x, y};
	}

	/**
	 * \brief Determines if the window is closed.
	 * \return \c true if the window is closed, otherwise \c false.
	 */
	[[nodiscard]] bool IsClosed() const noexcept {
		return glfwWindowShouldClose(window_);
	}

	/**
	 * \brief Determines if a key is pressed.
	 * \param key_code The key code to evaluate (e.g., GLFW_KEY_W).
	 * \return \c true if \p key is pressed, otherwise \c false.
	 */
	[[nodiscard]] bool IsKeyPressed(const int key_code) const noexcept {
		return glfwGetKey(window_, key_code) == GLFW_PRESS;
	}

	/**
	 * \brief Determines if a mouse button is pressed.
	 * \param button The mouse button code (e.g., GLFW_MOUSE_BUTTON_LEFT).
	 * \return \c true if \p button is pressed, otherwise \c false.
	 */
	[[nodiscard]] bool IsMouseButtonPressed(const int button) const noexcept {
		return glfwGetMouseButton(window_, button);
	}

	/** \brief Updates the window for the next iteration of main render loop. */
	void Update() const noexcept {
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

private:
	GLFWwindow* window_ = nullptr;
	std::function<void(int)> on_key_press_;
};
}
