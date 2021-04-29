#pragma once

#include <vector>

#include <GL/gl3w.h>

class Mesh {

public:
	Mesh(std::vector<GLfloat> positions, std::vector<GLuint> indices) noexcept
		: positions_{std::move(positions)}, indices_{std::move(indices)} {

		glGenVertexArrays(1, &vao_id_);
		glBindVertexArray(vao_id_);

		glGenBuffers(1, &vbo_id_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
		glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * positions_.size(), positions_.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &ebo_id_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof GLuint * indices_.size(), indices_.data(), GL_STATIC_DRAW);
	}

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	~Mesh() {
		glDeleteBuffers(1, &ebo_id_);
		glDeleteBuffers(1, &vbo_id_);
		glDeleteVertexArrays(1, &vao_id_);
	}

	void Draw() const noexcept {
		glBindVertexArray(vao_id_);
		glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
	}

private:
	GLuint vao_id_{0}, vbo_id_{0}, ebo_id_{0};
	std::vector<GLfloat> positions_;
	std::vector<GLuint> indices_;
};
