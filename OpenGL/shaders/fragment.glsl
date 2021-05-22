#version 460 core

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	vec3 attenuation;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform PointLight point_lights[2];
uniform Material material;

out vec4 fragment_color;

vec3 GetPointLightColor(const int index) {

	PointLight point_light = point_lights[index];

	vec3 light_direction = point_light.position - vertex.position.xyz;
	float light_distance = length(light_direction);
	light_direction = normalize(light_direction);

	float diffuse_intensity = max(dot(light_direction, vertex.normal), 0.0f);
	vec3 diffuse_color = diffuse_intensity * material.diffuse;

	if (diffuse_intensity > 0.0f) {

		vec3 view_direction = normalize(-vertex.position.xyz);
		vec3 reflect_direction = normalize(reflect(-light_direction, vertex.normal));
		float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);
		vec3 specular_color = specular_intensity * material.specular;

		float attenuation = dot(point_light.attenuation, vec3(1.0, light_distance, light_distance * light_distance));
		return point_light.color * point_light.intensity * (diffuse_color + specular_color) / attenuation;
	}

	return vec3(0.0f);
}

void main() {
	vec3 light_color = material.ambient;
	for (int i = 0; i < point_lights.length(); ++i) {
		light_color += GetPointLightColor(i);
	}
	fragment_color = vec4(light_color, 1.0f);
}