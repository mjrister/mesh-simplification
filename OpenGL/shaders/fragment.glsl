#version 460 core

uniform sampler2D image;

in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

out vec4 color;

void main() {
	color = texture(image, vertex.texture_coordinates);
}