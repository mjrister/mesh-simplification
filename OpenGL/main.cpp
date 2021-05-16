#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/mesh.h"
#include "graphics/obj_loader.h"
#include "graphics/point_light.h"
#include "graphics/shader_program.h"
#include "graphics/texture2d.h"
#include "graphics/window.h"

int main() {

	try {
		constexpr std::int32_t window_width{1280}, window_height{960};
		constexpr std::int32_t opengl_major_version{4}, opengl_minor_version{6};
		gfx::Window window{"OpenGL", window_width, window_height, opengl_major_version, opengl_minor_version};

		gfx::ShaderProgram shader_program{"shaders/vertex.glsl", "shaders/fragment.glsl"};
		shader_program.Enable();

		const gfx::Texture2d texture2d{"resources/textures/bob.png", GL_TEXTURE0};
		texture2d.Bind();

		auto mesh = gfx::obj_loader::LoadMesh("resources/models/bob.obj");
		mesh.Initialize();
		mesh.Scale(glm::vec3{0.5f});

		constexpr auto aspect_ratio = static_cast<GLfloat>(window_width) / window_height;
		const auto projection_transform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
		shader_program.SetUniform("projection_transform", projection_transform);

		constexpr glm::vec3 eye{0.0f, 0.0f, 2.0f}, center{0.0f}, up{0.0f, 1.0f, 0.0f};
		const auto view_transform = glm::lookAt(eye, center, up);

		constexpr glm::vec3 ambient_color{0.3f};
		shader_program.SetUniform("ambient_color", ambient_color);

		constexpr gfx::PointLight point_light{
			.position = glm::vec3{-1.0f, 1.0f, 1.0f},
			.color = glm::vec3{1.0f},
			.intensity = 1.0f,
			.attenuation = glm::vec3{0.0f, 0.0f, 1.0f}
		};

		shader_program.SetUniform("point_light.position", glm::mat3{view_transform} * point_light.position);
		shader_program.SetUniform("point_light.color", point_light.color);
		shader_program.SetUniform("point_light.intensity", point_light.intensity);
		shader_program.SetUniform("point_light.attenuation", point_light.attenuation);

		while (!window.Closed()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			window.HandleInput(mesh);

			const auto model_view_transform = view_transform * mesh.Model();
			shader_program.SetUniform("model_view_transform", model_view_transform);

			const auto normal_matrix = glm::inverse(transpose(glm::mat3{model_view_transform}));
			shader_program.SetUniform("normal_transform", normal_matrix);

			mesh.Render();
			window.Update();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
