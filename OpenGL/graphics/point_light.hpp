#pragma once

#include <glm/glm.hpp>

namespace gfx {

	struct PointLight {
		glm::vec3 position;
		glm::vec3 color;
		float intensity;
		glm::vec3 attenuation;
	};
}