#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "app/window.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace app {

class Scene {

public:
	Scene(Window& window, gfx::ShaderProgram& shader_program);
	void Render(float delta_time);

private:
	struct ViewFrustum {
		float field_of_view_y;
		float z_near;
		float z_far;
	};

	struct Camera {
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	};

	struct PointLight {
		glm::vec4 position;
		glm::vec3 color;
		glm::vec3 attenuation;
	};

	void InitializeCamera();
	void InitializeMesh();
	void InitializePointLights();

	void UpdateProjectionTransform();
	void HandleDiscreteKeyPress(int key_code);
	void HandleContinuousInput(float delta_time);

	static constexpr Camera kCamera{
		.eye = glm::vec3{0., 0., 2.f},
		.center = glm::vec3{0.f},
		.up = glm::vec3{0.f, 1.f, 0.f}
	};
	Window& window_;
	gfx::ShaderProgram& shader_program_;
	gfx::Mesh mesh_;
	glm::mat4 view_transform_;
};
}
