#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

out Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

void main() {
	vertex.position = model_view_matrix * vec4(position, 1.0f);
	vertex.texture_coordinates = texture_coordinates;
	vertex.normal = normalize(normal_matrix * normal);
	gl_Position = projection_matrix * vertex.position;
}