#version 410 core

in Fragment {
  vec3 position;
  vec3 normal;
  vec2 texcoords;
} fragment;

uniform struct PointLight {
  vec3 position;
  vec3 color;
} point_lights[8];

uniform int point_lights_size;

uniform struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
} material;

out vec4 fragment_color;

vec3 GetNormal() {
#ifdef ENABLE_SMOOTH_SHADING
  return normalize(vertex.normal);
#else
  return normalize(cross(dFdx(fragment.position.xyz), dFdy(fragment.position.xyz)));
#endif
}

vec3 GetLightDirection(const PointLight point_light, out float light_attenuation) {
  vec3 light_direction = point_light.position - fragment.position.xyz;
  float light_distance = length(light_direction);
  light_attenuation = 1.0f / max(light_distance * light_distance, 1.0);
  return normalize(light_direction);
}

vec3 GetDiffuseColor(const vec3 normal, const vec3 light_direction) {
  float diffuse_intensity = max(dot(light_direction, normal), 0.0);
  return material.diffuse * diffuse_intensity;
}

vec3 GetSpecularColor(const vec3 normal, const vec3 light_direction) {
  vec3 view_direction = normalize(-fragment.position.xyz);
  vec3 reflect_direction = normalize(reflect(-light_direction, normal));
  float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
  return material.specular * specular_intensity;
}

void main() {
  vec3 normal = GetNormal();
  fragment_color = vec4(material.ambient, 1.0);

  for (int i = 0; i < min(point_lights_size, point_lights.length()); ++i) {
    PointLight point_light = point_lights[i];
    float light_attenuation = 0.0;
    vec3 light_direction = GetLightDirection(point_light, light_attenuation);
    vec3 diffuse_color = GetDiffuseColor(normal, light_direction);
    vec3 specular_color = GetSpecularColor(normal, light_direction);
    fragment_color.xyz += light_attenuation * point_light.color * (diffuse_color + specular_color);
  }
}
