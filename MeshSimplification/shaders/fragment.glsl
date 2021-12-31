#version 460 core

in Vertex {
	vec4 position;
	vec3 normal;
} vertex;

struct PointLight {
	vec3 position;
	vec3 color;
	vec3 attenuation;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform PointLight point_lights[2];
uniform Material material;
uniform bool use_phong_shading;

out vec4 fragment_color;

void main() {
	vec3 vertex_position = vertex.position.xyz;
	vec3 vertex_normal = normalize(use_phong_shading ? vertex.normal : cross(dFdx(vertex_position), dFdy(vertex_position)));
	fragment_color = vec4(material.ambient, 1.f);

	for (int i = 0; i < point_lights.length(); ++i) {
		PointLight point_light = point_lights[i];

		vec3 light_direction = point_light.position - vertex_position;
		float light_distance = length(light_direction);
		float attenuation = 1.f / dot(point_light.attenuation, vec3(1.f, light_distance, pow(light_distance, 2.f)));

		light_direction = normalize(light_direction);
		float diffuse_intensity = max(dot(light_direction, vertex_normal), 0.f);
		vec3 diffuse_color = material.diffuse * diffuse_intensity;

		vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal));
		vec3 view_direction = normalize(-vertex_position);
		float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.f), material.shininess);
		vec3 specular_color = material.specular * specular_intensity;

		fragment_color += vec4(attenuation * point_light.color * (diffuse_color + specular_color), 0.f);
	}
}
