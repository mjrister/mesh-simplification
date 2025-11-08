#include "graphics/window.h"

#include <cassert>
#include <format>
#include <iostream>
#include <stdexcept>

namespace {

[[maybe_unused]] void APIENTRY HandleDebugMessageReceived(const GLenum source,
                                                          const GLenum type,
                                                          const GLuint id,
                                                          const GLenum severity,
                                                          const GLsizei /*length*/,
                                                          const GLchar* const message,
                                                          const void* /*user_param*/) {
  std::string message_source;
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      message_source = "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      message_source = "WINDOW SYSTEM";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      message_source = "SHADER COMPILER";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      message_source = "THIRD PARTY";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      message_source = "APPLICATION";
      break;
    default:
      message_source = "OTHER";
      break;
  }

  std::string message_type;
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      message_type = "ERROR";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      message_type = "DEPRECATED BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      message_type = "UNDEFINED BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      message_type = "PORTABILITY";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      message_type = "PERFORMANCE";
      break;
    default:
      message_type = "OTHER";
      break;
  }

  std::string message_severity;
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      message_severity = "HIGH";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      message_severity = "MEDIUM";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      message_severity = "LOW";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      message_severity = "NOTIFICATION";
      break;
    default:
      message_severity = "OTHER";
      break;
  }

  std::clog << format("OpenGL Debug ({}): Source: {}, Type: {}, Severity: {}\n{}\n",
                      id,
                      message_source,
                      message_type,
                      message_severity,
                      message);
}

void InitializeGlfw(const std::pair<const int, const int>& opengl_version) {
  if (glfwInit() == GLFW_FALSE) throw std::runtime_error{"GLFW initialization failed"};

  const auto [major_version, minor_version] = opengl_version;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwSwapInterval(1);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
  glfwSetErrorCallback([](const int error_code, const char* const description) {
    std::cerr << std::format("GLFW Error ({}): {}\n", error_code, description);
  });
#endif
}

void InitializeGl3w(const std::pair<const int, const int>& opengl_version) {
  if (gl3wInit() != GL3W_OK) throw std::runtime_error{"OpenGL initialization failed"};

  if (const auto [major_version, minor_version] = opengl_version; gl3wIsSupported(major_version, minor_version) == 0) {
    throw std::runtime_error{std::format("OpenGL {}.{} not supported", major_version, minor_version)};
  }

#ifndef NDEBUG
  // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
  std::clog << std::format("OpenGL version: {}, GLSL version: {}\n",
                           reinterpret_cast<const char*>(glGetString(GL_VERSION)),
                           reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
  // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(HandleDebugMessageReceived, nullptr);
#endif

  // configure OpenGL graphics pipeline state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);
}
}  // namespace

qem::Window::Window(const char* const title,
                    const std::pair<int, int>& window_dimensions,
                    const std::pair<int, int>& opengl_version) {
  InitializeGlfw(opengl_version);

  const auto [width, height] = window_dimensions;
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window_ == nullptr) throw std::runtime_error{"Window creation failed"};

  glfwSetWindowUserPointer(window_, this);
  glfwMakeContextCurrent(window_);

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* /*window*/, const int new_width, const int new_height) {
    glViewport(0, 0, new_width, new_height);
  });

  glfwSetKeyCallback(
      window_,
      [](GLFWwindow* const window, const int key, const int /*scancode*/, const int action, const int /*modifiers*/) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (action == GLFW_PRESS) {
          const auto* const self = static_cast<Window*>(glfwGetWindowUserPointer(window));
          assert(self != nullptr);
          if (self->on_key_press_) self->on_key_press_(key);
        }
      });

  glfwSetScrollCallback(window_, [](GLFWwindow* const window, const double x_offset, const double y_offset) {
    const auto* const self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    assert(self != nullptr);
    if (self->on_scroll_) self->on_scroll_(x_offset, y_offset);
  });

  InitializeGl3w(opengl_version);
}

qem::Window::~Window() noexcept {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
  glfwTerminate();
}
