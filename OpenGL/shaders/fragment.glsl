#version 410 core

// interpolated data from the vertex shader
in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

// a light source at a fixed position in space whose rays shine in all directions
uniform struct PointLight {
	vec4 position; // assumed to be in camera space
	vec3 color;
	float intensity;
	vec3 attenuation; // (constant, linear, exponential) attenuation coefficients
} point_light;

// light reflectance properties for a material
uniform struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

out vec4 fragment_color;

// computes the fragment color using the Blinn-Phong reflection model
void main() {

	// start initial light contribution off with ambient intensity
	vec3 light_color = material.ambient;

	vec3 light_direction = (point_light.position - vertex.position).xyz;
	float light_distance = length(light_direction);
	light_direction = normalize(light_direction);

	// calculate diffuse intensity
	vec3 normal = normalize(vertex.normal);
	float diffuse_intensity = max(dot(light_direction, normal), 0.f);

	// avoid calculating specular intensity if angle between light source and vertex position is greater than 90 degrees
	if (diffuse_intensity > 0.f) {
		vec3 diffuse_color = diffuse_intensity * material.diffuse;

		// calculate specular intensity
		vec3 view_direction = normalize(-vertex.position.xyz);
		vec3 halfway_direction = normalize(light_direction + view_direction);
		float specular_intensity = pow(max(dot(halfway_direction, normal), 0.f), material.shininess);
		vec3 specular_color = specular_intensity * material.specular;

		// account for light attenuation
		float attenuation = dot(point_light.attenuation, vec3(1.f, light_distance, light_distance * light_distance));
		light_color += point_light.color * point_light.intensity * (diffuse_color + specular_color) / attenuation;
	}

	fragment_color = vec4(light_color, 1.f);
}
