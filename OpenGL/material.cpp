#include "material.h"

// Material properties courtesy of http://devernay.free.fr/cours/opengl/materials.html

constexpr gfx::Material gfx::Material::Emerald() {
	constexpr glm::vec3 ambient = {0.0215f, 0.1745, 0.0215f};
	constexpr glm::vec3 diffuse = {0.07568f, 0.61424f, 0.07568f};
	constexpr glm::vec3 specular = {0.633f, 0.727811f, 0.633f};
	constexpr GLfloat shininess = 0.6f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Jade() {
	constexpr glm::vec3 ambient = {0.135f, 0.2225f, 0.1575f};
	constexpr glm::vec3 diffuse = {0.54f, 0.89f, 0.63f};
	constexpr glm::vec3 specular = {0.316228f, 0.316228f, 0.316228f};
	constexpr GLfloat shininess = 0.1f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Obsidian() {
	constexpr glm::vec3 ambient = {0.05375f, 0.05f, 0.06625f};
	constexpr glm::vec3 diffuse = {0.18275f, 0.17f, 0.22525f};
	constexpr glm::vec3 specular = {0.332741f, 0.328634f, 0.346435f};
	constexpr GLfloat shininess = 0.3f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Pearl() {
	constexpr glm::vec3 ambient = {0.25f, 0.20725f, 0.20725f};
	constexpr glm::vec3 diffuse = {1.0f, 0.829f, 0.829f};
	constexpr glm::vec3 specular = {0.296648f, 0.296648f, 0.296648f};
	constexpr GLfloat shininess = 0.088f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Ruby() {
	const glm::vec3 ambient = {0.1745f, 0.01175f, 0.01175f};
	const glm::vec3 diffuse = {0.61424f, 0.04136f, 0.04136f};
	const glm::vec3 specular = {0.727811f, 0.626959f, 0.626959f};
	constexpr GLfloat shininess = 0.6f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Turquoise() {
	constexpr glm::vec3 ambient = {0.1f, 0.18725f, 0.1745f};
	constexpr glm::vec3 diffuse = {0.396f, 0.74151f, 0.69102f};
	constexpr glm::vec3 specular = {0.297254f, 0.30829f, 0.306678f};
	constexpr GLfloat shininess = 0.1f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Brass() {
	constexpr glm::vec3 ambient = {0.329412f, 0.223529f, 0.027451f};
	constexpr glm::vec3 diffuse = {0.780392f, 0.568627f, 0.113725f};
	constexpr glm::vec3 specular = {0.992157f, 0.941176f, 0.807843f};
	constexpr GLfloat shininess = 0.21794872f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Bronze() {
	constexpr glm::vec3 ambient = {0.2125f, 0.1275f, 0.054f};
	constexpr glm::vec3 diffuse = {0.714f, 0.4284f, 0.18144f};
	constexpr glm::vec3 specular = {0.393548f, 0.271906f, 0.166721f};
	constexpr GLfloat shininess = 0.2f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Chrome() {
	constexpr glm::vec3 ambient = {0.25f, 0.25f, 0.25f};
	constexpr glm::vec3 diffuse = {0.4f, 0.4f, 0.4f};
	constexpr glm::vec3 specular = {0.774597f, 0.774597f, 0.774597f};
	constexpr GLfloat shininess = 0.6f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Copper() {
	constexpr glm::vec3 ambient = {0.19125f, 0.0735f, 0.0225f};
	constexpr glm::vec3 diffuse = {0.7038f, 0.27048f, 0.0828f};
	constexpr glm::vec3 specular = {0.256777f, 0.137622f, 0.086014f};
	constexpr GLfloat shininess = 0.1f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Gold() {
	constexpr glm::vec3 ambient = {0.24725f, 0.1995f, 0.0745f};
	constexpr glm::vec3 diffuse = {0.75164f, 0.60648f, 0.22648f};
	constexpr glm::vec3 specular = {0.628281f, 0.555802f, 0.366065f};
	constexpr GLfloat shininess = 0.4f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::Silver() {
	constexpr glm::vec3 ambient = {0.19225f, 0.19225f, 0.19225f};
	constexpr glm::vec3 diffuse = {0.50754f, 0.50754f, 0.50754f};
	constexpr glm::vec3 specular = {0.508273f, 0.508273f, 0.508273f};
	constexpr GLfloat shininess = 0.4f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::BlackPlastic() {
	constexpr glm::vec3 ambient = {0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.01f, 0.01f, 0.01f};
	constexpr glm::vec3 specular = {0.50f, 0.50f, 0.50f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::CyanPlastic() {
	constexpr glm::vec3 ambient = {0.0, 0.1, 0.06};
	constexpr glm::vec3 diffuse = {0.0, 0.50980392, 0.50980392};
	constexpr glm::vec3 specular = {0.50196078f, 0.50196078f, 0.50196078f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::GreenPlastic() {
	constexpr glm::vec3 ambient = {0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.1f, 0.35f, 0.1f};
	constexpr glm::vec3 specular = {0.45f, 0.55f, 0.45f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::RedPlastic() {
	constexpr glm::vec3 ambient = {0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.5f, 0.0f, 0.0f};
	constexpr glm::vec3 specular = {0.7f, 0.6f, 0.6f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::WhitePlastic() {
	constexpr glm::vec3 ambient = {0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.55f, 0.55f, 0.55f};
	constexpr glm::vec3 specular = {0.7f, 0.7f, 0.7f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::YellowPlastic() {
	constexpr glm::vec3 ambient = {0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.5f, 0.5f, 0.0f};
	constexpr glm::vec3 specular = {0.6f, 0.6f, 0.5f};
	constexpr GLfloat shininess = 0.25f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::BlackRubber() {
	constexpr glm::vec3 ambient = {0.02f, 0.02f, 0.02f};
	constexpr glm::vec3 diffuse = {0.01f, 0.01f, 0.01f};
	constexpr glm::vec3 specular = {0.4f, 0.4f, 0.4f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::CyanRubber() {
	constexpr glm::vec3 ambient = {0.0f, 0.05f, 0.05f};
	constexpr glm::vec3 diffuse = {0.4f, 0.5f, 0.5f};
	constexpr glm::vec3 specular = {0.04f, 0.7f, 0.7f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::GreenRubber() {
	constexpr glm::vec3 ambient = {0.0f, 0.05f, 0.0f};
	constexpr glm::vec3 diffuse = {0.4f, 0.5f, 0.4f};
	constexpr glm::vec3 specular = {0.04f, 0.7f, 0.4f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::RedRubber() {
	constexpr glm::vec3 ambient = {0.05f, 0.0f, 0.0f};
	constexpr glm::vec3 diffuse = {0.5f, 0.4f, 0.4f};
	constexpr glm::vec3 specular = {0.7f, 0.04f, 0.04f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::WhiteRubber() {
	constexpr glm::vec3 ambient = {0.05f, 0.05f, 0.05f};
	constexpr glm::vec3 diffuse = {0.5f, 0.5f, 0.5f};
	constexpr glm::vec3 specular = {0.7f, 0.7f, 0.7f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}

constexpr gfx::Material gfx::Material::YellowRubber() {
	constexpr glm::vec3 ambient = {0.05f, 0.05f, 0.0f};
	constexpr glm::vec3 diffuse = {0.5f, 0.5f, 0.4f};
	constexpr glm::vec3 specular = {0.7f, 0.7f, 0.04f};
	constexpr GLfloat shininess = 0.078125f;
	return Material{ambient, diffuse, specular, shininess};
}
