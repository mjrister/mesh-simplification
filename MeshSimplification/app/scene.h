#pragma once

#include <array>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "app/window.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace app {

class Scene {

public:
	explicit Scene(Window& window, gfx::ShaderProgram& shader_program);
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

	struct SceneObject {
		gfx::Mesh mesh;
		gfx::Material material;
	};

	struct PointLight {
		glm::vec4 position;
		glm::vec3 color;
		glm::vec3 attenuation;
	};

	void UpdateProjectionTransform();
	void HandleDiscreteKeyPress(int key_code);
	void HandleContinuousInput(float delta_time);

	static constexpr ViewFrustum kViewFrustrum{
		.field_of_view_y = glm::radians(45.f),
		.z_near = .1f,
		.z_far = 100.f
	};
	static constexpr Camera kCamera{
		.eye = glm::vec3{0., 0., 2.f},
		.center = glm::vec3{0.f},
		.up = glm::vec3{0.f, 1.f, 0.f}
	};
	static constexpr std::array<PointLight, 2> kPointLights{
		PointLight{
			.position = glm::vec4{1.f, 1.f, 1.f, 1.f},
			.color = glm::vec3{1.f},
			.attenuation = glm::vec3{0.f, 0.f, 1.f}
		},
		PointLight{
			.position = glm::vec4{-1.f, 1.f, 2.f, 1.f},
			.color = glm::vec3{1.f},
			.attenuation = glm::vec3{0.f, 0.f, 1.f}
		}
	};
	Window& window_;
	gfx::ShaderProgram& shader_program_;
	std::vector<SceneObject> scene_objects_;
	int active_scene_object_ = 0;
	glm::mat4 view_transform_;
};
}
