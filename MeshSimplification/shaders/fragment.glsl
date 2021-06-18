#version 460 core

// interpolated data from the vertex shader
in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

// a light source at a fixed position in camera space whose rays shine in all directions
uniform struct PointLight {
	vec3 position;
	vec3 color;
	vec3 attenuation;
} point_light;

// light reflectance properties for a material
uniform struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

out vec4 fragment_color;

void main() {

	// use triangle normal to give a more flat effect
	vec3 vertex_position = vertex.position.xyz;
	vec3 vertex_normal =  normalize(cross(dFdx(vertex_position), dFdy(vertex_position)));

	// calculate light attenuation
	vec3 light_direction = point_light.position - vertex_position;
	float light_distance = length(light_direction);
	float attenuation = 1.f / dot(point_light.attenuation, vec3(1.f, light_distance, pow(light_distance, 2.f)));

	// calculate angle between light source and vertex normal
	light_direction = normalize(light_direction);
	float diffuse_intensity = max(dot(light_direction, vertex_normal), 0.f);
	vec3 diffuse_color = diffuse_intensity * material.diffuse;

	// calculate the angle between the reflected light and view direction
	vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal));
	vec3 view_direction = normalize(-vertex_position);
	float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.f), material.shininess);
	vec3 specular_color = specular_intensity * material.specular;

	// calculate light contributing using the Phong reflection model
	fragment_color = vec4(material.ambient + point_light.color * attenuation * (diffuse_color + specular_color), 1.f);
}
