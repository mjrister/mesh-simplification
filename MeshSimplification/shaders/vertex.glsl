#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 projection_transform;
uniform mat4 view_model_transform;

out Vertex {
	vec4 position;
	vec3 normal;
} vertex;

void main() {

	vertex.position = view_model_transform * vec4(position, 1.f);
	gl_Position = projection_transform * vertex.position;

	// generally, normals should be transformed by the upper 3x3 inverse transpose of the view model matrix. In this context,
	// it is sufficient to use the view-model matrix to transform normals because meshes are only transformed by rotations
	// and translations (which are orthogonal matrices with the property that their inverse is equal to their transpose) in
	// addition to uniform unscaling which is undone when the transformed normal is renomalized.
	vertex.normal = normalize(mat3(view_model_transform) * normal);
}