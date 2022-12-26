#pragma once

#include <stdexcept>

#include <glm/vec3.hpp>

namespace qem {

/**
 * \brief An enumeration for various material types.
 * \see Material::FromType
 */
enum class MaterialType {
	kBrass,
	kBronze,
	kChrome,
	kCopper,
	kEmerald,
	kGold,
	kJade,
	kObsidian,
	kPearl,
	kRuby,
	kSilver,
	kTurquoise
};

/**
 * \brief A type of material with light reflectance properties used in the Phong reflection model.
 * \note Material properties courtesy of http://devernay.free.fr/cours/opengl/materials.html
 */
struct Material {

	/**
	 * \brief A static factory function to construct a material from a dedicated material type.
	 * \param material_type The material type representing the material to construct.
	 * \return A material containing light reflectance properties for the provided material type.
	 */
	static constexpr Material FromType(const MaterialType material_type) {
		switch (material_type) {
			case MaterialType::kBrass: {
				return Material{
					.ambient = glm::vec3{0.329412f, 0.223529f, 0.027451f},
					.diffuse = glm::vec3{0.780392f, 0.568627f, 0.113725f},
					.specular = glm::vec3{0.992157f, 0.941176f, 0.807843f},
					.shininess = 0.21794872f,
				};
			}
			case MaterialType::kBronze: {
				return Material{
					.ambient = glm::vec3{0.2125f, 0.1275f, 0.054f},
					.diffuse = glm::vec3{0.714f, 0.4284f, 0.18144f},
					.specular = glm::vec3{0.393548f, 0.271906f, 0.166721f},
					.shininess = 0.2f
				};
			}
			case MaterialType::kChrome: {
				return Material{
					.ambient = glm::vec3{0.25f, 0.25f, 0.25f},
					.diffuse = glm::vec3{0.4f, 0.4f, 0.4f},
					.specular = glm::vec3{0.774597f, 0.774597f, 0.774597f},
					.shininess = 0.6f
				};
			}
			case MaterialType::kCopper: {
				return Material{
					.ambient = glm::vec3{0.19125f, 0.0735f, 0.0225f},
					.diffuse = glm::vec3{0.7038f, 0.27048f, 0.0828f},
					.specular = glm::vec3{0.256777f, 0.137622f, 0.086014f},
					.shininess = 0.6f
				};
			}
			case MaterialType::kEmerald: {
				return Material{
					.ambient = glm::vec3{0.0215f, 0.1745f, 0.0215f},
					.diffuse = glm::vec3{0.07568f, 0.61424f, 0.07568f},
					.specular = glm::vec3{0.633f, 0.727811f, 0.633f},
					.shininess = 0.6f
				};
			}
			case MaterialType::kGold: {
				return Material{
					.ambient = glm::vec3{0.24725f, 0.1995f, 0.0745f},
					.diffuse = glm::vec3{0.75164f, 0.60648f, 0.22648f},
					.specular = glm::vec3{0.628281f, 0.555802f, 0.366065f},
					.shininess = 0.4f
				};
			}
			case MaterialType::kJade: {
				return Material{
					.ambient = glm::vec3{0.135f, 0.2225f, 0.1575f},
					.diffuse = glm::vec3{0.54f, 0.89f, 0.63f},
					.specular = glm::vec3{0.316228f, 0.316228f, 0.316228f},
					.shininess = 0.1f
				};
			}
			case MaterialType::kObsidian: {
				return Material{
					.ambient = glm::vec3{0.05375f, 0.05f, 0.06625f},
					.diffuse = glm::vec3{0.18275f, 0.17f, 0.22525f},
					.specular = glm::vec3{0.332741f, 0.328634f, 0.346435f},
					.shininess = 0.3f
				};
			}
			case MaterialType::kPearl: {
				return Material{
					.ambient = glm::vec3{0.25f, 0.20725f, 0.20725f},
					.diffuse = glm::vec3{1.0f, 0.829f, 0.829f},
					.specular = glm::vec3{0.296648f, 0.296648f, 0.296648f},
					.shininess = 0.088f
				};
			}
			case MaterialType::kRuby: {
				return Material{
					.ambient = glm::vec3{0.1745f, 0.01175f, 0.01175f},
					.diffuse = glm::vec3{0.61424f, 0.04136f, 0.04136f},
					.specular = glm::vec3{0.727811f, 0.626959f, 0.626959f},
					.shininess = 0.6f
				};
			}
			case MaterialType::kSilver: {
				return Material{
					.ambient = glm::vec3{0.19225f, 0.19225f, 0.19225f},
					.diffuse = glm::vec3{0.50754f, 0.50754f, 0.50754f},
					.specular = glm::vec3{0.508273f, 0.508273f, 0.508273f},
					.shininess = 0.4f
				};
			}
			case MaterialType::kTurquoise: {
				return Material{
					.ambient = glm::vec3{0.1f, 0.18725f, 0.1745f},
					.diffuse = glm::vec3{0.396f, 0.74151f, 0.69102f},
					.specular = glm::vec3{0.297254f, 0.30829f, 0.306678f},
					.shininess = 0.1f
				};
			}
			default:
				assert(false);
				return Material{};
		}
	}

	/** The ambient light reflected. */
	glm::vec3 ambient;

	/** The diffuse light reflected. */
	glm::vec3 diffuse;

	/** The specular light reflected. */
	glm::vec3 specular;

	/** The degree of shininess reflected from specular highlights. */
	float shininess;
};
}
