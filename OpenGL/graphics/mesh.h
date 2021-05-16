#pragma once

#include <vector>

#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

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
		[[nodiscard]] const auto& Model() const noexcept { return model_; }

		void Initialize() noexcept;
		void Render() const noexcept;

		void Scale(const glm::vec3 xyz) { model_ = glm::scale(model_, xyz); }
		void Rotate(const GLfloat angle, const glm::vec3 axis) { model_ = glm::rotate(model_, angle, axis); }
		void Translate(const glm::vec3 xyz) { model_ = glm::translate(model_, xyz); }

	private:
		GLuint vertex_array_{}, vertex_buffer_{}, element_buffer_{};
		const std::vector<glm::vec3> positions_;
		const std::vector<glm::vec2> texture_coordinates_;
		const std::vector<glm::vec3> normals_;
		const std::vector<GLuint> indices_;
		glm::mat4 model_{1.0f};
	};
}
