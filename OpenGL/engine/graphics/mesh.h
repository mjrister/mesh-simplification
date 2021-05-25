#pragma once

#include <vector>

#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

namespace gfx {

	class Mesh {

	public:

		/**
		 * \brief Initializes a triangle mesh.
		 * \param positions The mesh vertex positions.
		 * \param texture_coordinates The mesh texture coordinates.
		 * \param normals The mesh normals.
		 * \param indices Element indices such that each three consecutive integers define a triangle face in the mesh.
	         * \throw std::invalid_argument Indicates the provided arguments do not represent a valid triangle mesh.
		 * \note If \p indices is empty, \p positions must describe a triangle mesh (i.e., be a nonzero multiple of 3).
		 *       If nonempty, \p texture_coordinates and \p normals must be the same size as \p positions so that data
		 *       is aligned when sent to the vertex shader. If \p indices if nonempty, it must describe a triangle mesh,
		 *       however, \p positions, \p texture_coordinates, and \p normals may be of any size. Consequentially, each
		 *       index assumes alignment between \p positions and \p texture_coordinates, \p normals.
		 */
		explicit Mesh(
			std::vector<glm::vec4> positions,
			std::vector<glm::vec2> texture_coordinates = {},
			std::vector<glm::vec3> normals = {},
			std::vector<GLuint> indices = {});
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;

		/** \brief Gets the mesh vertex positions. */
		[[nodiscard]] const auto& Positions() const noexcept { return positions_; }

		/** \brief Gets the mesh texture coordinates. */
		[[nodiscard]] const auto& TextureCoordinates() const noexcept { return texture_coordinates_; }

		/** \brief Gets the mesh normals. */
		[[nodiscard]] const auto& Normals() const noexcept { return normals_; }

		/** \brief Gets the mesh indices. */
		[[nodiscard]] const auto& Indices() const noexcept { return indices_; }

		/** \brief Gets the mesh model transform in local object space. */
		[[nodiscard]] const auto& Model() const noexcept { return model_; }

		/** \brief Renders the mesh to the current framebuffer. */
		void Render() const noexcept {
			glBindVertexArray(vertex_array_);
			if (element_buffer_) {
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
			} else {
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions_.size()));
			}
			glBindVertexArray(0);
		}

		/**
		 * \brief Scales the mesh in local object space.
		 * \param xyz The x,y,z directions to scale the mesh.
		 */
		void Scale(const glm::vec3& xyz) { model_ = glm::scale(model_, xyz); }

		/**
		 * \brief Rotates the mesh in local object space.
		 * \param axis The axis to rotate the mesh about.
		 * \param angle The rotation angle specified in radians.
		 */
		void Rotate(const glm::vec3& axis, const GLfloat angle) { model_ = glm::rotate(model_, angle, axis); }


		/**
		 * \brief Translates the mesh in local object space.
		 * \param xyz The x,y,z directions to translate the mesh.
		 */
		void Translate(const glm::vec3& xyz) { model_ = glm::translate(model_, xyz); }

	private:
		GLuint vertex_array_{}, vertex_buffer_{}, element_buffer_{};
		const std::vector<glm::vec4> positions_;
		const std::vector<glm::vec2> texture_coordinates_;
		const std::vector<glm::vec3> normals_;
		const std::vector<GLuint> indices_;
		glm::mat4 model_{1.f};
	};
}
