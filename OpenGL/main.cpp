#include <iostream>
#include <sstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Window {

public:
	Window(const char* const title, const int width, const int height) {
		InitializeGlfw();
		window_ = CreateGlfwWindow(title, width, height);
		InitializeGl3w();
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

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version_);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version_);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	static GLFWwindow* CreateGlfwWindow(const char* const title, const int width, const int height) {

		if (auto* window = glfwCreateWindow(width, height, title, nullptr, nullptr)) {
			glfwMakeContextCurrent(window);
			return window;
		}

		throw std::runtime_error{"Window creation failed"};
	}

	static void InitializeGl3w() {

		if (gl3wInit()) {
			throw std::runtime_error{"OpenGL initialization failed"};
		}

		if (!gl3wIsSupported(major_version_, minor_version_)) {
			std::ostringstream oss;
			oss << "OpenGL " << major_version_ << "." << minor_version_ << "not supported";
			throw std::runtime_error{oss.str()};
		}
	}

	static constexpr int major_version_ = 4;
	static constexpr int minor_version_ = 6;
	GLFWwindow* window_;
};

int main() {

	try {
		const Window window{"OpenGL", 640, 480};

		while (!window.Closed()) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			window.SwapBuffers();
			glfwPollEvents();
		}

	} catch (const std::runtime_error e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
