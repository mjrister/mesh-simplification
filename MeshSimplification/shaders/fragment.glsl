#version 460 core

// interpolated data from the vertex shader
in Vertex {
	vec4 position;
	vec2 texture_coordinates;
	vec3 normal;
} vertex;

// a light source at a fixed position in space whose rays shine in all directions
uniform struct PointLight {
	vec4 position; // in camera space
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

// evaluates the fragment color using the Phong reflection model
vec4 GetFragmentColor() {

	// start initial light contribution off with ambient intensity
	vec3 fragment_color = material.ambient;

	vec3 light_direction = (point_light.position - vertex.position).xyz;
	float light_distance = length(light_direction);
	light_direction = normalize(light_direction);

	// calculate angle between light source and triangle normal
	vec3 triangle_normal = normalize(cross(dFdx(vertex.position.xyz), dFdy(vertex.position.xyz)));
	float diffuse_intensity = max(dot(light_direction, triangle_normal), 0.f);

	// avoid calculating specular intensity if angle between light source and vertex position is greater than 90 degrees
	if (diffuse_intensity > 0.f) {
		vec3 diffuse_color = diffuse_intensity * material.diffuse;

		// calculate the angle between the reflected light and view direction
		vec3 reflect_direction = normalize(reflect(-light_direction, triangle_normal));
		vec3 view_direction = normalize(-vertex.position.xyz);
		float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.f), material.shininess);
		vec3 specular_color = specular_intensity * material.specular;

		// account for light attenuation
		float attenuation = 1.f / dot(point_light.attenuation, vec3(1.f, light_distance, pow(light_distance, 2.f)));
		fragment_color += point_light.color * attenuation * (diffuse_color + specular_color);
	}

	return vec4(fragment_color, 1.f);
}


void main() {
	fragment_color = GetFragmentColor();
}
