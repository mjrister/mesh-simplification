#include <exception>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "app/scene.h"
#include "app/window.h"
#include "graphics/shader_program.h"

using namespace app;
using namespace gfx;
using namespace std;

int main() {

	try {
		constexpr auto kWindowDimensions = make_pair(1280, 960);
		constexpr auto kOpenGlVersion = make_pair(4, 6);
		Window window{"Mesh Simplification", kWindowDimensions, kOpenGlVersion};
		ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		Scene scene{&window, &shader_program};

		for (auto previous_time = static_cast<float>(glfwGetTime()), delta_time = 0.f; !window.IsClosed();) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			const auto current_time = static_cast<float>(glfwGetTime());
			delta_time = current_time - previous_time;
			previous_time = current_time;

			scene.Render(delta_time);
			window.Update();
		}
	} catch (const exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
