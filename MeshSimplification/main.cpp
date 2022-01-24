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
		constexpr auto window_dimensions = make_pair(1280, 960);
		constexpr auto opengl_version = make_pair(4, 6);
		Window window{"Mesh Simplification", window_dimensions, opengl_version};
		ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		Scene scene{window, shader_program};

		for (auto previous_time = glfwGetTime(), delta_time = 0.; !window.Closed();) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene.Render(static_cast<float>(delta_time));
			window.Update();

			const auto current_time = glfwGetTime();
			delta_time = current_time - previous_time;
			previous_time = current_time;
		}
	} catch (const exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
