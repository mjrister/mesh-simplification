#version 460 core

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

uniform struct Attenuation {
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

float GetPointLightAttenuation(vec3 light_direction) {
	float distance = length(light_direction);
	float constant = point_light.attenuation.constant;
	float linear = point_light.attenuation.linear * distance;
	float exponent = point_light.attenuation.exponent * pow(distance, 2.0);
	return constant + linear + exponent;
}

vec3 GetPointLightColor(vec3 texture_color) {
	vec3 light_direction = point_light.position - vertex.position.xyz;
	vec3 diffuse_color = GetDiffuseColor(normalize(light_direction), point_light.color, point_light.intensity);
	vec3 light_color = texture_color * diffuse_color;
	float attenuation = GetPointLightAttenuation(light_direction);
	return light_color / attenuation;
}

void main() {
	vec3 light_direction = point_light.position - vertex.position.xyz;
	vec3 diffuse_color = GetDiffuseColor(normalize(light_direction), point_light.color, point_light.intensity);
	vec3 texture_color = texture(image, vertex.texture_coordinates).rgb;
	vec3 light_color = texture_color * diffuse_color;
	float attenuation = GetPointLightAttenuation(light_direction);
	fragment_color = vec4(light_color / attenuation, 1.0f);
}