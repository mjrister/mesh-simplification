#ifndef GRAPHICS_WINDOW_H_
#define GRAPHICS_WINDOW_H_

#include <concepts>
#include <functional>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace gfx {

class Window {
public:
  Window(const char* title, const std::pair<int, int>& window_size, const std::pair<int, int>& opengl_version);

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(Window&&) noexcept = delete;
  Window& operator=(Window&&) noexcept = delete;

  ~Window() noexcept;

  template <std::invocable<int, int> Fn>
  void OnKeyEvent(Fn&& key_event_handler) {
    key_event_handler_ = std::forward<Fn>(key_event_handler);
  }

  template <std::invocable<float, float> Fn>
  void OnCursorEvent(Fn&& cursor_event_handler) {
    cursor_event_handler_ = std::forward<Fn>(cursor_event_handler);
  }

  template <std::invocable<float> Fn>
  void OnScrollEvent(Fn&& scroll_event_handler) {
    scroll_event_handler_ = std::forward<Fn>(scroll_event_handler);
  }

  [[nodiscard]] bool IsMouseButtonPressed(const int button) const noexcept {
    return glfwGetMouseButton(window_, button) == GLFW_PRESS;
  }

  [[nodiscard]] std::pair<int, int> GetSize() const noexcept {
    auto width = 0, height = 0;
    glfwGetWindowSize(window_, &width, &height);
    return std::pair{width, height};
  }

  [[nodiscard]] float GetAspectRatio() const noexcept {
    const auto [width, height] = GetSize();
    return height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 0;
  }

  void Close() const noexcept { glfwSetWindowShouldClose(window_, GLFW_TRUE); }

  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(window_) == GLFW_TRUE; }

  void Update() noexcept {
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

private:
  GLFWwindow* window_ = nullptr;
  std::function<void(int, int)> key_event_handler_;
  std::function<void(float, float)> cursor_event_handler_;
  std::function<void(float)> scroll_event_handler_;
};

}  // namespace gfx

#endif  // GRAPHICS_WINDOW_H_
