#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;

uniform mat4 model_view_projection;

out Vertex {
	vec2 texture_coordinates;
} vertex;

void main() {
	gl_Position = model_view_projection * vec4(position, 1.0f);
	vertex.texture_coordinates = texture_coordinates;
}