#version 460 core

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

uniform struct PointLight {
	vec4 position;
	vec3 color;
	float intensity;
	vec3 attenuation;
} point_light;

uniform struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

out vec4 fragment_color;

void main() {
	vec3 light_color = material.ambient;
	vec3 light_direction = vec3(point_light.position - vertex.position);
	float light_distance = length(light_direction);
	light_direction = normalize(light_direction);
	vec3 normal = normalize(vertex.normal);
	float diffuse_intensity = max(dot(light_direction, normal), 0.f);

	if (diffuse_intensity > 0.f) {
		vec3 diffuse_color = diffuse_intensity * material.diffuse;
		vec3 reflect_direction = normalize(reflect(-light_direction, normal));
		vec3 view_direction = normalize(-vertex.position.xyz);
		float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.f), material.shininess);
		vec3 specular_color = specular_intensity * material.specular;
		float attenuation = dot(point_light.attenuation, vec3(1.f, light_distance, light_distance * light_distance));
		light_color += point_light.color * point_light.intensity * (diffuse_color + specular_color) / attenuation;
	}

	fragment_color = vec4(light_color, 1.f);
}