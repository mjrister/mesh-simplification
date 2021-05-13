#version 460 core

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

uniform struct AmbientLight {
	vec3 color;
	float intensity;
} ambient_light;

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

uniform struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	Attenuation attenuation;
} point_light;

uniform sampler2D image;

out vec4 fragment_color;

vec3 GetDiffuseColor(vec3 light_direction, vec3 light_color, float light_intensity) {
	float diffuse_intensity = max(dot(light_direction, vertex.normal), 0.0f);
	return light_color * light_intensity * diffuse_intensity;
}

vec3 GetSpecularColor(vec3 light_direction, vec3 light_color, float light_intensity, float shininess) {
	vec3 view_direction = normalize(-vertex.position.xyz);
	vec3 reflect_direction = normalize(reflect(-light_direction, vertex.normal));
	float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0f), shininess);
	return light_color * light_intensity * specular_intensity;
}

vec3 GetLightColor(vec3 texture_color, vec3 light_direction, vec3 light_color, float light_intensity) {
	vec3 ambient_color = ambient_light.color * ambient_light.intensity;
	vec3 diffuse_color = GetDiffuseColor(light_direction, point_light.color, point_light.intensity);
	vec3 specular_color = GetSpecularColor(light_direction, light_color, light_intensity, 100);
	return texture_color * (ambient_color + diffuse_color) + specular_color;
}

float GetPointLightAttenuation(vec3 light_direction) {
	float distance = length(light_direction);
	float constant = point_light.attenuation.constant;
	float linear = point_light.attenuation.linear * distance;
	float exponent = point_light.attenuation.exponent * pow(distance, 2.0);
	return constant + linear + exponent;
}

vec3 GetPointLightColor(PointLight light, vec3 texture_color) {
	vec3 light_direction = light.position - vertex.position.xyz;
	vec3 light_color = GetLightColor(texture_color, normalize(light_direction), light.color, light.intensity);
	float attenuation = GetPointLightAttenuation(light_direction);
	return light_color / attenuation;
}

void main() {
	vec3 texture_color = texture(image, vertex.texture_coordinates).rgb;
	vec3 light_color = GetPointLightColor(point_light, texture_color);
	fragment_color = vec4(light_color, 1.0f);
}