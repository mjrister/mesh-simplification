#version 410 core

in Vertex {
  vec3 position;
  vec2 texture_coordiantes;
  vec3 normal;
} vertex;

uniform struct PointLight {
  vec3 position;
  vec3 color;
  vec3 attenuation;
} point_lights[8];

uniform int point_lights_size;

uniform struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
} material;

out vec4 fragment_color;

void main() {
  vec3 vertex_normal = normalize(cross(dFdx(vertex.position.xyz), dFdy(vertex.position.xyz)));
  fragment_color = vec4(material.ambient, 1.0);

  for (int i = 0; i < min(point_lights_size, point_lights.length()); ++i) {
    PointLight point_light = point_lights[i];

    vec3 light_direction = point_light.position - vertex.position.xyz;
    float light_distance = length(light_direction);
    float attenuation_inv = dot(point_light.attenuation, vec3(1.0, light_distance, pow(light_distance, 2.0)));
    float attenuation = 1.0 / max(attenuation_inv, 1.0);
    light_direction = normalize(light_direction);

    float diffuse_intensity = max(dot(light_direction, vertex_normal), 0.0);
    vec3 diffuse_color = material.diffuse * diffuse_intensity;

    vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal));
    vec3 view_direction = normalize(-vertex.position.xyz);
    float specular_intensity = pow(max(dot(reflect_direction, view_direction), 0.0), material.shininess);
    vec3 specular_color = material.specular * specular_intensity;

    fragment_color += vec4(attenuation * point_light.color * (diffuse_color + specular_color), 0.0);
  }
}
