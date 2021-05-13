#pragma once

#include <glm/glm.hpp>

struct AmbientLight {
	glm::vec3 color;
	float intensity;
};

struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	struct Attenuation {
		float constant;
		float linear;
		float exponent;
	} attenuation;
};