#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 projection_transform;
uniform mat4 model_view_transform;

out Fragment {
  vec3 position;
  vec3 normal;
  vec2 texcoords;
} fragment;

void main() {
  fragment.position = vec3(model_view_transform * vec4(position, 1.0));
  fragment.normal = normalize(mat3(model_view_transform) * normal); // model-view transform assumed to be orthogonal
  fragment.texcoords = texcoords;
  gl_Position = projection_transform * vec4(fragment.position, 1.0);
}
