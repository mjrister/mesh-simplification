#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 projection_transform;
uniform mat4 model_view_transform;

out Vertex {
  vec3 position;
  vec2 texture_coordiantes;
  vec3 normal;
} vertex;

void main() {
  vertex.position = vec3(model_view_transform * vec4(position, 1.0));
  vertex.texture_coordiantes = texture_coordinates;
  vertex.normal = normalize(mat3(model_view_transform) * normal);
  gl_Position = projection_transform * vec4(vertex.position, 1.0);
}
