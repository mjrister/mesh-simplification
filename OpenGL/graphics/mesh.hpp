#pragma once

#include <vector>

#include <GL/gl3w.h>

class Mesh {

public:
	explicit Mesh(
		std::vector<GLfloat> positions,
		std::vector<GLfloat> texture_coordinates = {},
		std::vector<GLfloat> normals = {},
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
		glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * positions_.size(), positions_.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		if (!texture_coordinates_.empty()) {
			glGenBuffers(1, &texture_coordinates_vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * texture_coordinates_.size(), texture_coordinates_.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(1);
		}

		if (!normals_.empty()) {
			glGenBuffers(1, &normals_vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, normals_vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * normals_.size(), normals_.data(), GL_STATIC_DRAW);
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
		if (!indices_.empty()) {
			glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, positions_.size() / 3);
		}
		glBindVertexArray(0);
	}

private:
	static void Validate(
		const std::vector<GLfloat>& positions,
		const std::vector<GLfloat>& texture_coordinates,
		const std::vector<GLfloat>& normals,
		const std::vector<GLuint>& indices) {

		if (positions.empty()) throw std::invalid_argument{"Vertex positions must be specified"};
		if (positions.size() % 3 != 0) throw std::invalid_argument{"Vertex positions must be 3D"};
		if (texture_coordinates.size() % 2 != 0) throw std::invalid_argument{"Texture coordinates must be 2D"};
		if (normals.size() % 3 != 0) throw std::invalid_argument{"Vertex normals must be 3D"};
		if (indices.size() % 3 != 0) throw std::invalid_argument{"Object must be a triangle mesh"};

		if (!texture_coordinates.empty() && positions.size() / 3 != texture_coordinates.size() / 2) {
			throw std::invalid_argument{"Texture coordinates must align with position data."};
		}
		if (!normals.empty() && positions.size() != normals.size()) {
			throw std::invalid_argument{"Vertex normals must align with position data."};
		}
	}

	GLuint vao_{0}, positions_vbo_{0}, texture_coordinates_vbo_{0}, normals_vbo_{0}, ebo_{0};
	std::vector<GLfloat> positions_, texture_coordinates_, normals_;
	std::vector<GLuint> indices_;
};
