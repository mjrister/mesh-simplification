#version 460 core

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

uniform struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	vec3 attenuation;
} point_light;

uniform vec3 ambient_color;
uniform sampler2D image;

out vec4 fragment_color;

void main() {
	vec3 light_direction = point_light.position - vertex.position.xyz;
	float light_distance = length(light_direction);
	float attenuation = 1.0f / dot(point_light.attenuation, vec3(1.0, light_distance, pow(light_distance, 2.0f)));

	light_direction = normalize(light_direction);
	float diffuse_intensity = max(dot(light_direction, vertex.normal), 0.0f);
	vec3 diffuse_color = point_light.color * point_light.intensity * diffuse_intensity;

	vec3 view_direction = normalize(-vertex.position.xyz);
	vec3 reflect_direction = normalize(reflect(-light_direction, vertex.normal));
	float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0f), 100.0f);
	vec3 specular_color =  point_light.color * point_light.intensity * specular_intensity;

	vec3 texture_color = texture(image, vertex.texture_coordinates).rgb;
	vec3 light_color = texture_color * (ambient_color + diffuse_color) + specular_color;
	fragment_color = vec4(light_color * attenuation, 1.0f);
}