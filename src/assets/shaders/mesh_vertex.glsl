#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 model_view_transform;
uniform mat4 projection_transform;

out Fragment {
  vec3 position;
  vec3 normal;
  vec2 texcoords;
} fragment;

void main() {
  vec4 view_position = model_view_transform * vec4(position, 1.0);
  fragment.position = vec3(view_position);
  fragment.normal = normalize(mat3(model_view_transform) * normal); // model-view transform assumed to be orthogonal
  fragment.texcoords = texcoords;
  gl_Position = projection_transform * view_position;
}
