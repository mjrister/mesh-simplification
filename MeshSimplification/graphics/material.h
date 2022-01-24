#pragma once

#include <stdexcept>

#include <glm/vec3.hpp>

namespace gfx {

/**
 * \brief An enumeration for various material types.
 * \see Material::FromType
 */
enum class MaterialType :uint8_t {
	Brass,
	Bronze,
	Chrome,
	Copper,
	Emerald,
	Gold,
	Jade,
	Obsidian,
	Pearl,
	Ruby,
	Silver,
	Turquoise
};

/** \brief A type of material with light reflectance properties. */
class Material {

public:
	/**
	 * \brief A static factory function to construct a material from a dedicated material type.
	 * \param material_type The material type representing the material to construct.
	 * \return A material containing light reflectance properties for the provided material type.
	 */
	static constexpr Material FromType(const MaterialType material_type) {
		switch (material_type) {
			case MaterialType::Brass: {
				constexpr glm::vec3 ambient{.329412f, .223529f, .027451f};
				constexpr glm::vec3 diffuse{.780392f, .568627f, .113725f};
				constexpr glm::vec3 specular{.992157f, .941176f, .807843f};
				constexpr auto shininess{.21794872f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Bronze: {
				constexpr glm::vec3 ambient{.2125f, .1275f, .054f};
				constexpr glm::vec3 diffuse{.714f, .4284f, .18144f};
				constexpr glm::vec3 specular{.393548f, .271906f, .166721f};
				constexpr auto shininess{.2f};
				return Material{ambient, diffuse, specular, shininess};
			};
			case MaterialType::Chrome: {
				constexpr glm::vec3 ambient{.25f, .25f, .25f};
				constexpr glm::vec3 diffuse{.4f, .4f, .4f};
				constexpr glm::vec3 specular{.774597f, .774597f, .774597f};
				constexpr auto shininess{.6f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Copper: {
				constexpr glm::vec3 ambient{.19125f, .0735f, .0225f};
				constexpr glm::vec3 diffuse{.7038f, .27048f, .0828f};
				constexpr glm::vec3 specular{.256777f, .137622f, .086014f};
				constexpr auto shininess{.1f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Emerald: {
				constexpr glm::vec3 ambient{.0215f, .1745f, .0215f};
				constexpr glm::vec3 diffuse{.07568f, .61424f, .07568f};
				constexpr glm::vec3 specular{.633f, .727811f, .633f};
				constexpr auto shininess{.6f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Gold: {
				constexpr glm::vec3 ambient{.24725f, .1995f, .0745f};
				constexpr glm::vec3 diffuse{.75164f, .60648f, .22648f};
				constexpr glm::vec3 specular{.628281f, .555802f, .366065f};
				constexpr auto shininess{.4f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Jade: {
				constexpr glm::vec3 ambient{.135f, .2225f, .1575f};
				constexpr glm::vec3 diffuse{.54f, .89f, .63f};
				constexpr glm::vec3 specular{.316228f, .316228f, .316228f};
				constexpr auto shininess{.1f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Obsidian: {
				constexpr glm::vec3 ambient{.05375f, .05f, .06625f};
				constexpr glm::vec3 diffuse{.18275f, .17f, .22525f};
				constexpr glm::vec3 specular{.332741f, .328634f, .346435f};
				constexpr auto shininess{.3f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Pearl: {
				constexpr glm::vec3 ambient{.25f, .20725f, .20725f};
				constexpr glm::vec3 diffuse{1.f, .829f, .829f};
				constexpr glm::vec3 specular{.296648f, .296648f, .296648f};
				constexpr auto shininess{.088f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Ruby: {
				constexpr glm::vec3 ambient{.1745f, .01175f, .01175f};
				constexpr glm::vec3 diffuse{.61424f, .04136f, .04136f};
				constexpr glm::vec3 specular{.727811f, .626959f, .626959f};
				constexpr auto shininess{.6f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Silver: {
				constexpr glm::vec3 ambient{.19225f, .19225f, .19225f};
				constexpr glm::vec3 diffuse{.50754f, .50754f, .50754f};
				constexpr glm::vec3 specular{.508273f, .508273f, .508273f};
				constexpr auto shininess{.4f};
				return Material{ambient, diffuse, specular, shininess};
			}
			case MaterialType::Turquoise: {
				constexpr glm::vec3 ambient{.1f, .18725f, .1745f};
				constexpr glm::vec3 diffuse{.396f, .74151f, .69102f};
				constexpr glm::vec3 specular{.297254f, .30829f, .306678f};
				constexpr auto shininess{.1f};
				return Material{ambient, diffuse, specular, shininess};
			}
			default:
				throw std::invalid_argument{"Invalid material type"};
		}
	}

	/**
	 * \brief Initializes a material.
	 * \param ambient The ambient light reflected.
	 * \param diffuse The diffuse light reflected.
	 * \param specular The specular light reflected.
	 * \param shininess The degree of shininess reflected from specular highlights.
	 */
	constexpr Material(
		const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float shininess)
		: ambient_{ambient},
		  diffuse_{diffuse},
		  specular_{specular},
		  shininess_{shininess} {}

	/** \brief Gets the ambient light reflected. */
	[[nodiscard]] constexpr glm::vec3 Ambient() const { return ambient_; }

	/** \brief Gets the diffuse light reflected. */
	[[nodiscard]] constexpr glm::vec3 Diffuse() const { return diffuse_; }

	/** \brief Gets the specular light reflected. */
	[[nodiscard]] constexpr glm::vec3 Specular() const { return specular_; }

	/** \brief Gets the  degree of shininess reflected from specular highlights. */
	[[nodiscard]] constexpr float Shininess() const { return shininess_; }

private:
	const glm::vec3 ambient_;
	const glm::vec3 diffuse_;
	const glm::vec3 specular_;
	const float shininess_;
};
}
