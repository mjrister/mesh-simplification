#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "app/window.h"
#include "graphics/material.h"
#include "graphics/mesh.h"

namespace gfx {
	class ShaderProgram;
}

namespace app {

	class Scene {

	public:
		explicit Scene(Window& window);
		void Render(gfx::ShaderProgram& shader_program, float delta_time);

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

		void OnKeyPress(int32_t key_code);
		void HandleInput(float delta_time);

		Window& window_;
		ViewFrustum view_frustum_;
		Camera camera_;
		std::vector<PointLight> point_lights_;
		std::vector<SceneObject> scene_objects_;
		std::int32_t active_scene_object_ = 0;
	};
}
