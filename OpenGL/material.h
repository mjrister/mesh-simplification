#pragma once
#include <GL/gl3w.h>
#include <glm/vec3.hpp>

namespace gfx {

	class Material {

	public:
		constexpr Material(
			const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const GLfloat shininess)
			: ambient_{ambient},
			  diffuse_{diffuse},
			  specular_{specular},
			  shininess_{shininess} {}

		[[nodiscard]] constexpr glm::vec3 Ambient() const noexcept { return ambient_; }
		[[nodiscard]] constexpr glm::vec3 Diffuse() const noexcept { return diffuse_; }
		[[nodiscard]] constexpr glm::vec3 Specular() const noexcept { return specular_; }
		[[nodiscard]] constexpr GLfloat Shininess() const noexcept { return shininess_; }

		static constexpr Material Emerald();
		static constexpr Material Jade();
		static constexpr Material Obsidian();
		static constexpr Material Pearl();
		static constexpr Material Ruby();
		static constexpr Material Turquoise();
		static constexpr Material Brass();
		static constexpr Material Bronze();
		static constexpr Material Chrome();
		static constexpr Material Copper();
		static constexpr Material Gold();
		static constexpr Material Silver();

		static constexpr Material BlackPlastic();
		static constexpr Material CyanPlastic();
		static constexpr Material GreenPlastic();
		static constexpr Material RedPlastic();
		static constexpr Material WhitePlastic();
		static constexpr Material YellowPlastic();

		static constexpr Material BlackRubber();
		static constexpr Material CyanRubber();
		static constexpr Material GreenRubber();
		static constexpr Material RedRubber();
		static constexpr Material WhiteRubber();
		static constexpr Material YellowRubber();

	private:
		glm::vec3 ambient_;
		glm::vec3 diffuse_;
		glm::vec3 specular_;
		GLfloat shininess_;
	};
}
