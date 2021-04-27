#pragma once

#include <vector>

#include <GL/gl3w.h>

template <std::size_t N>
class Mesh {

public:
	Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices) {

		glGenVertexArrays(1, &vao_id_);
		glBindVertexArray(vao_id_);

		glGenBuffers(1, &vbo_id_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
		glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &ebo_id_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof GLuint * indices.size(), indices.data(), GL_STATIC_DRAW);
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

	void Draw() const {
		glBindVertexArray(vao_id_);
		glDrawElements(GL_TRIANGLES, (N - 2) * 3, GL_UNSIGNED_INT, nullptr);
	}

private:
	GLuint vao_id_{0}, vbo_id_{0}, ebo_id_{0};
};
