#pragma once

#include <stdexcept>

#include <glm/vec3.hpp>

namespace gfx {

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
 * \brief A type of material with light reflectance properties.
 * \note  Material properties courtesy of http://devernay.free.fr/cours/opengl/materials.html
 */
class Material {

public:
	/**
	 * \brief A static factory function to construct a material from a dedicated material type.
	 * \param material_type The material type representing the material to construct.
	 * \return A material containing light reflectance properties for the provided material type.
	 */
	static constexpr Material FromType(const MaterialType material_type) {
		switch (material_type) {
			case MaterialType::kBrass: {
				constexpr glm::vec3 kAmbient{.329412f, .223529f, .027451f};
				constexpr glm::vec3 kDiffuse{.780392f, .568627f, .113725f};
				constexpr glm::vec3 kSpecular{.992157f, .941176f, .807843f};
				constexpr auto kShininess = .21794872f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kBronze: {
				constexpr glm::vec3 kAmbient{.2125f, .1275f, .054f};
				constexpr glm::vec3 kDiffuse{.714f, .4284f, .18144f};
				constexpr glm::vec3 kSpecular{.393548f, .271906f, .166721f};
				constexpr auto kShininess = .2f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			};
			case MaterialType::kChrome: {
				constexpr glm::vec3 kAmbient{.25f, .25f, .25f};
				constexpr glm::vec3 kDiffuse{.4f, .4f, .4f};
				constexpr glm::vec3 kSpecular{.774597f, .774597f, .774597f};
				constexpr auto kShininess = .6f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kCopper: {
				constexpr glm::vec3 kAmbient{.19125f, .0735f, .0225f};
				constexpr glm::vec3 kDiffuse{.7038f, .27048f, .0828f};
				constexpr glm::vec3 kSpecular{.256777f, .137622f, .086014f};
				constexpr auto kShininess = .1f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kEmerald: {
				constexpr glm::vec3 kAmbient{.0215f, .1745f, .0215f};
				constexpr glm::vec3 kDiffuse{.07568f, .61424f, .07568f};
				constexpr glm::vec3 kSpecular{.633f, .727811f, .633f};
				constexpr auto kShininess = .6f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kGold: {
				constexpr glm::vec3 kAmbient{.24725f, .1995f, .0745f};
				constexpr glm::vec3 kDiffuse{.75164f, .60648f, .22648f};
				constexpr glm::vec3 kSpecular{.628281f, .555802f, .366065f};
				constexpr auto kShininess = .4f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kJade: {
				constexpr glm::vec3 kAmbient{.135f, .2225f, .1575f};
				constexpr glm::vec3 kDiffuse{.54f, .89f, .63f};
				constexpr glm::vec3 kSpecular{.316228f, .316228f, .316228f};
				constexpr auto kShininess = .1f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kObsidian: {
				constexpr glm::vec3 kAmbient{.05375f, .05f, .06625f};
				constexpr glm::vec3 kDiffuse{.18275f, .17f, .22525f};
				constexpr glm::vec3 kSpecular{.332741f, .328634f, .346435f};
				constexpr auto kShininess = .3f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kPearl: {
				constexpr glm::vec3 kAmbient{.25f, .20725f, .20725f};
				constexpr glm::vec3 kDiffuse{1.f, .829f, .829f};
				constexpr glm::vec3 kSpecular{.296648f, .296648f, .296648f};
				constexpr auto kShininess = .088f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kRuby: {
				constexpr glm::vec3 kAmbient{.1745f, .01175f, .01175f};
				constexpr glm::vec3 kDiffuse{.61424f, .04136f, .04136f};
				constexpr glm::vec3 kSpecular{.727811f, .626959f, .626959f};
				constexpr auto kShininess = .6f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kSilver: {
				constexpr glm::vec3 kAmbient{.19225f, .19225f, .19225f};
				constexpr glm::vec3 kDiffuse{.50754f, .50754f, .50754f};
				constexpr glm::vec3 kSpecular{.508273f, .508273f, .508273f};
				constexpr auto kShininess = .4f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
			}
			case MaterialType::kTurquoise: {
				constexpr glm::vec3 kAmbient{.1f, .18725f, .1745f};
				constexpr glm::vec3 kDiffuse{.396f, .74151f, .69102f};
				constexpr glm::vec3 kSpecular{.297254f, .30829f, .306678f};
				constexpr auto kShininess = .1f;
				return Material{kAmbient, kDiffuse, kSpecular, kShininess};
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
	[[nodiscard]] constexpr glm::vec3 ambient() const { return ambient_; }

	/** \brief Gets the diffuse light reflected. */
	[[nodiscard]] constexpr glm::vec3 diffuse() const { return diffuse_; }

	/** \brief Gets the specular light reflected. */
	[[nodiscard]] constexpr glm::vec3 specular() const { return specular_; }

	/** \brief Gets the  degree of shininess reflected from specular highlights. */
	[[nodiscard]] constexpr float shininess() const { return shininess_; }

private:
	glm::vec3 ambient_;
	glm::vec3 diffuse_;
	glm::vec3 specular_;
	float shininess_;
};
}
