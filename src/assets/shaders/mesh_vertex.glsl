#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 projection_transform;
uniform mat4 model_view_transform;

out Fragment {
  vec3 position;
  vec2 texture_coordinates;
  vec3 normal;
} fragment;

void main() {
  fragment.position = vec3(model_view_transform * vec4(position, 1.0));
  fragment.texture_coordinates = texture_coordinates;
  fragment.normal = normalize(mat3(model_view_transform) * normal); // model-view transform assumed to be orthogonal
  gl_Position = projection_transform * vec4(fragment.position, 1.0);
}
