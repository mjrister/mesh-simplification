#include "mesh.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace {

	// Ensures that the provided positions, texture coordinates, normals and indices describe a triangle mesh
	void Validate(
		const std::vector<glm::vec4>& positions,
		const std::vector<glm::vec2>& texture_coordinates,
		const std::vector<glm::vec3>& normals,
		const std::vector<GLuint>& indices) {

		if (positions.empty()) {
			throw std::invalid_argument{"Vertex positions must be specified"};
		}
		if (indices.empty() && positions.size() % 3 != 0 || indices.size() % 3 != 0) {
			throw std::invalid_argument{"Object must be a triangle mesh"};
		}
		if (indices.empty() && !texture_coordinates.empty() && positions.size() != texture_coordinates.size()) {
			throw std::invalid_argument{"Texture coordinates must align with position data"};
		}
		if (indices.empty() && !normals.empty() && positions.size() != normals.size()) {
			throw std::invalid_argument{"Vertex normals must align with position data"};
		}
	}
}

gfx::Mesh::Mesh(
	std::vector<glm::vec4> positions,
	std::vector<glm::vec2> texture_coordinates,
	std::vector<glm::vec3> normals,
	std::vector<GLuint> indices)
	: positions_{std::move(positions)},
	  texture_coordinates_{std::move(texture_coordinates)},
	  normals_{std::move(normals)},
	  indices_{std::move(indices)} {

	Validate(positions_, texture_coordinates_, normals_, indices_);

	// generate vertex array
	glGenVertexArrays(1, &vertex_array_);
	glBindVertexArray(vertex_array_);

	// generate vertex buffer
	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

	// allocate memory for vertex buffer
	const std::size_t positions_size = sizeof(glm::vec4) * positions_.size();
	const std::size_t texture_coordinates_size = sizeof(glm::vec2) * texture_coordinates_.size();
	const std::size_t normals_size = sizeof(glm::vec3) * normals_.size();
	const std::size_t buffer_size = positions_size + texture_coordinates_size + normals_size;
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

	// copy positions to vertex buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions_size, positions_.data());
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	// copy texture coordinates to vertex buffer
	if (!texture_coordinates_.empty()) {
		glBufferSubData(GL_ARRAY_BUFFER, positions_size, texture_coordinates_size, texture_coordinates_.data());
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(positions_size));
		glEnableVertexAttribArray(1);
	}

	// copy normals to vertex buffer
	if (!normals_.empty()) {
		const std::size_t offset = positions_size + texture_coordinates_size;
		glBufferSubData(GL_ARRAY_BUFFER, offset, normals_size, normals_.data());
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(offset));
		glEnableVertexAttribArray(2);
	}

	// generate and copy indices to element buffer
	if (!indices_.empty()) {
		const std::size_t indices_size = sizeof(GLuint) * indices_.size();
		glGenBuffers(1, &element_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices_.data(), GL_STATIC_DRAW);
	}
}

gfx::Mesh::~Mesh() {
	glDeleteBuffers(1, &element_buffer_);
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteVertexArrays(1, &vertex_array_);
}