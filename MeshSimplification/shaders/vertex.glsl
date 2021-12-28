#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 projection_transform;
uniform mat4 view_model_transform;
uniform mat3 normal_transform;

out Vertex {
	vec4 position;
	vec3 normal;
} vertex;

void main() {
	vertex.position = view_model_transform * vec4(position, 1.f);
	vertex.normal = normalize(normal_transform * normal);
	gl_Position = projection_transform * vertex.position;
}