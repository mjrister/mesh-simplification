#version 460 core

uniform sampler2D image;

in Vertex {
	vec2 texture_coordinates;
} vertex;

out vec4 color;

void main() {
	color = texture(image, vertex.texture_coordinates);
}