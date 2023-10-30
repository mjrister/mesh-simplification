#include <gtest/gtest.h>

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace {

void InitializeGl3w() {
  if (gl3wInit() != GL3W_OK) {
    throw std::runtime_error{"OpenGL initialization failed"};
  }
}

void InitializeGlfw() {
  if (glfwInit() == GLFW_FALSE) throw std::runtime_error{"GLFW initialization failed"};
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
}

GLFWwindow* CreateGlfwWindow() {
  if (auto* window = glfwCreateWindow(100, 100, "Test", nullptr, nullptr)) {
    glfwMakeContextCurrent(window);
    return window;
  }
  throw std::runtime_error{"Window creation failed"};
}

}  // namespace

int main(int argc, char** argv) {
  try {
    InitializeGlfw();
    auto* const window = CreateGlfwWindow();
    InitializeGl3w();
    testing::InitGoogleTest(&argc, argv);
    const auto exit_code = RUN_ALL_TESTS();
    glfwDestroyWindow(window);
    glfwTerminate();
    return exit_code;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred" << std::endl;
    return EXIT_FAILURE;
  }
}
