#pragma once

#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace gfx {

	class Mesh {

	public:
		explicit Mesh(
			std::vector<glm::vec3> positions,
			std::vector<glm::vec2> texture_coordinates = {},
			std::vector<glm::vec3> normals = {},
			std::vector<GLuint> indices = {});
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;


		[[nodiscard]] const auto& Positions() const noexcept { return positions_; }
		[[nodiscard]] const auto& TextureCoordinates() const noexcept { return texture_coordinates_; }
		[[nodiscard]] const auto& Normals() const noexcept { return normals_; }
		[[nodiscard]] const auto& Indices() const noexcept { return indices_; }

		void Initialize() noexcept;
		void Render() const noexcept;

	private:
		GLuint vertex_array_{0}, vertex_buffer_{0}, element_buffer_{0};
		const std::vector<glm::vec3> positions_;
		const std::vector<glm::vec2> texture_coordinates_;
		const std::vector<glm::vec3> normals_;
		const std::vector<GLuint> indices_;
	};
}
