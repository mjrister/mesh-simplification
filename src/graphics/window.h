#ifndef GRAPHICS_WINDOW_H_
#define GRAPHICS_WINDOW_H_

#include <concepts>
#include <functional>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace gfx {

/**
 * @brief An abstraction for a GLFW window.
 * @see https://www.glfw.org/documentation GLFW Documentation
 */
class Window {
public:
  /**
   * @brief Creates a window.
   * @param title The UTF-8 window title.
   * @param window_dimensions The window width and height.
   * @param opengl_version The OpenGL major and minor version.
   * @throw std::runtime_error Thrown if GLFW window creation fails.
   */
  Window(const char* title, const std::pair<int, int>& window_dimensions, const std::pair<int, int>& opengl_version);

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(Window&&) noexcept = delete;
  Window& operator=(Window&&) noexcept = delete;

  ~Window() noexcept;

  /**
   * @brief Gets the window size in screen coordinates.
   * @return The window width and height.
   */
  [[nodiscard]] std::pair<int, int> GetSize() const noexcept {
    auto width = 0, height = 0;
    glfwGetWindowSize(window_, &width, &height);
    return std::pair{width, height};
  }

  /**
   * @brief Gets the windows aspect ratio.
   * @return The ratio of the windows width to its height (e.g., 16/9).
   */
  [[nodiscard]] float GetAspectRatio() const noexcept {
    const auto [width, height] = GetSize();
    return height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 0;
  }

  /**
   * @brief Gets the cursor position.
   * @return The (x,y) coordinates of the cursor position in the window.
   */
  [[nodiscard]] glm::vec2 GetCursorPosition() const noexcept {
    auto x = 0.0, y = 0.0;
    glfwGetCursorPos(window_, &x, &y);
    return glm::vec2{static_cast<float>(x), static_cast<float>(y)};
  }

  /** @brief Sets the window close flag. */
  void Close() const noexcept { glfwSetWindowShouldClose(window_, GLFW_TRUE); }

  /**
   * @brief Checks if the window close flag has been set.
   * @return @c true if the window close flag has been set, otherwise @c false.
   */
  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(window_) == GLFW_TRUE; }

  /**
   * @brief Registers a callback to be invoked when a key press event is detected.
   * @param on_key_press The callback to be invoked on key press parameterized by the active key code.
   */
  template <std::invocable<int> Fn>
  void OnKeyPress(Fn&& on_key_press) {
    on_key_press_ = std::forward<Fn>(on_key_press);
  }

  /**
   * @brief Registers a callback to be invoked when a scroll event is detected.
   * @param on_scroll A callback to be invoked on scroll parameterized by x/y scroll offsets (respectively).
   */
  template <std::invocable<float, float> Fn>
  void OnScroll(Fn&& on_scroll) {
    on_scroll_ = std::forward<Fn>(on_scroll);
  }

  /**
   * @brief Checks if a key is pressed.
   * @param key The GLFW key to check (e.g., GLFW_KEY_ESCAPE).
   * @return @c true if @p key is pressed, otherwise @c false.
   */
  [[nodiscard]] bool IsKeyPressed(const int key) const noexcept { return glfwGetKey(window_, key) == GLFW_PRESS; }

  /**
   * @brief Checks if a mouse button is pressed.
   * @param button The GLFW mouse button to check (e.g., GLFW_MOUSE_BUTTON_LEFT).
   * @return @c true if @p button is pressed, otherwise @c false.
   */
  [[nodiscard]] bool IsMouseButtonPressed(const int button) const noexcept {
    return glfwGetMouseButton(window_, button) == GLFW_PRESS;
  }

  /** @brief Updates the window for the next iteration of main render loop. */
  void Update() noexcept {
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

private:
  GLFWwindow* window_ = nullptr;
  std::function<void(int)> on_key_press_;
  std::function<void(float, float)> on_scroll_;
};

}  // namespace gfx

#endif  // GRAPHICS_WINDOW_H_
