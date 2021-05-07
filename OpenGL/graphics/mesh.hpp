#pragma once

#include <vector>

#include <GL/gl3w.h>

class Mesh {

public:
	Mesh(std::vector<GLfloat> positions,
	     std::vector<GLfloat> texture_coordinates,
	     std::vector<GLfloat> normals,
	     std::vector<GLuint> indices) noexcept
		: positions_{std::move(positions)},
		  texture_coordinates_{std::move(texture_coordinates)},
		  normals_{std::move(normals)},
		  indices_{std::move(indices)} {}

	Mesh(std::vector<GLfloat> positions, std::vector<GLfloat> texture_coordinates, std::vector<GLuint> indices = {}) noexcept
		: positions_{std::move(positions)},
		  texture_coordinates_{std::move(texture_coordinates)},
		  indices_{std::move(indices)} {

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(1, &positions_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, positions_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * positions_.size(), positions_.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &texture_coordinates_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * texture_coordinates_.size(), texture_coordinates_.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);

		if (!indices_.empty()) {
			glGenBuffers(1, &ebo_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof GLuint* indices_.size(), indices_.data(), GL_STATIC_DRAW);
		}
	}

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	~Mesh() {
		if (ebo_) glDeleteBuffers(1, &ebo_);
		if (texture_coordinates_vbo_) glDeleteBuffers(1, &texture_coordinates_vbo_);
		if (positions_vbo_) glDeleteBuffers(1, &positions_vbo_);
		if (vao_) glDeleteVertexArrays(1, &vao_);
	}

	[[nodiscard]] const auto& Positions() const noexcept { return positions_; }
	[[nodiscard]] const auto& TextureCoordinates() const noexcept { return texture_coordinates_; }
	[[nodiscard]] const auto& Normals() const noexcept { return normals_; }
	[[nodiscard]] const auto& Indices() const noexcept { return indices_; }

	void Render() const noexcept {
		glBindVertexArray(vao_);
		if (ebo_) {
			glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, positions_.size() / 3);
		}
		glBindVertexArray(0);
	}

private:
	GLuint vao_{0}, positions_vbo_{0}, texture_coordinates_vbo_{0}, ebo_{0};
	std::vector<GLfloat> positions_, texture_coordinates_, normals_;
	std::vector<GLuint> indices_;
};
