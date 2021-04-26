#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Window {

public:
	Window(const char* const title, const int width, const int height) {
		InitializeGlfw();
		window_ = CreateGlfwWindow(title, width, height);
		InitializeGl3w();
		glEnable(GL_DEPTH_TEST);
	}

	~Window() {
		if (window_) {
			glfwDestroyWindow(window_);
		}
		glfwTerminate();
	}

	Window(const Window&) = delete;
	Window(Window&&) noexcept = delete;

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) noexcept = delete;

	[[nodiscard]] bool Closed() const noexcept {
		return glfwWindowShouldClose(window_);
	}

	void SwapBuffers() const noexcept {
		glfwSwapBuffers(window_);
	}

private:
	static void InitializeGlfw() {

		glfwSetErrorCallback([](const int error, const char* const description) {
			std::cerr << "Error " << error << ": " << description << std::endl;
		});

		if (!glfwInit()) {
			throw std::runtime_error{"GLFW initialization failed"};
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version_);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version_);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	static GLFWwindow* CreateGlfwWindow(const char* const title, const int width, const int height) {

		if (auto* window = glfwCreateWindow(width, height, title, nullptr, nullptr)) {
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, HandleWindowResize);
			glfwSetKeyCallback(window, HandleKeyEvent);
			glfwSwapInterval(1);
			return window;
		}

		throw std::runtime_error{"Window creation failed"};
	}

	static void InitializeGl3w() {

		if (gl3wInit()) {
			throw std::runtime_error{"OpenGL initialization failed"};
		}

		if (!gl3wIsSupported(opengl_major_version_, opengl_minor_version_)) {
			std::ostringstream oss;
			oss << "OpenGL " << opengl_major_version_ << "." << opengl_minor_version_ << "not supported";
			throw std::runtime_error{oss.str()};
		}
	}

	static void HandleWindowResize(GLFWwindow* window, const int width, const int height) noexcept {
		glViewport(0, 0, width, height);
	}

	static void HandleKeyEvent(
		GLFWwindow* window, const int key, const int scancode, const int action, const int modifiers) noexcept {

		if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	static constexpr int opengl_major_version_ = 4, opengl_minor_version_ = 6;
	GLFWwindow* window_;
};
