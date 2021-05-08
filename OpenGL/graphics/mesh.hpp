#pragma once

#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>

class Mesh {

public:
	explicit Mesh(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec2> texture_coordinates = {},
		std::vector<glm::vec3> normals = {},
		std::vector<GLuint> indices = {})
		: positions_{std::move(positions)},
		  texture_coordinates_{std::move(texture_coordinates)},
		  normals_{std::move(normals)},
		  indices_{std::move(indices)} {

		Validate(positions_, texture_coordinates_, normals_, indices_);
	}

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	~Mesh() {
		if (ebo_) glDeleteBuffers(1, &ebo_);
		if (positions_vbo_) glDeleteBuffers(1, &positions_vbo_);
		if (texture_coordinates_vbo_) glDeleteBuffers(1, &texture_coordinates_vbo_);
		if (normals_vbo_) glDeleteBuffers(1, &normals_vbo_);
		if (vao_) glDeleteVertexArrays(1, &vao_);
	}

	[[nodiscard]] const auto& Positions() const noexcept { return positions_; }
	[[nodiscard]] const auto& TextureCoordinates() const noexcept { return texture_coordinates_; }
	[[nodiscard]] const auto& Normals() const noexcept { return normals_; }
	[[nodiscard]] const auto& Indices() const noexcept { return indices_; }

	void Initialize() noexcept {

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(1, &positions_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, positions_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof glm::vec3 * positions_.size(), positions_.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		if (!texture_coordinates_.empty()) {
			glGenBuffers(1, &texture_coordinates_vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof glm::vec2 * texture_coordinates_.size(), texture_coordinates_.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(1);
		}

		if (!normals_.empty()) {
			glGenBuffers(1, &normals_vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, normals_vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof glm::vec3 * normals_.size(), normals_.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(2);
		}

		if (!indices_.empty()) {
			glGenBuffers(1, &ebo_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof GLuint * indices_.size(), indices_.data(), GL_STATIC_DRAW);
		}
	}

	void Render() const noexcept {
		glBindVertexArray(vao_);
		if (ebo_) {
			glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, positions_.size());
		}
		glBindVertexArray(0);
	}

private:
	static void Validate(
		const std::vector<glm::vec3>& positions,
		const std::vector<glm::vec2>& texture_coordinates,
		const std::vector<glm::vec3>& normals,
		const std::vector<GLuint>& indices) {

		if (positions.empty()) throw std::invalid_argument{"Vertex positions must be specified"};
		if (indices.size() % 3 != 0) throw std::invalid_argument{"Object must be a triangle mesh"};

		if (!texture_coordinates.empty() && positions.size() != texture_coordinates.size()) {
			throw std::invalid_argument{"Texture coordinates must align with position data"};
		}
		if (!normals.empty() && positions.size() != normals.size()) {
			throw std::invalid_argument{"Vertex normals must align with position data"};
		}
	}

	GLuint vao_{0};
	GLuint positions_vbo_{0}, texture_coordinates_vbo_{0}, normals_vbo_{0};
	GLuint ebo_{0};
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec2> texture_coordinates_;
	std::vector<glm::vec3> normals_;
	std::vector<GLuint> indices_;
};
