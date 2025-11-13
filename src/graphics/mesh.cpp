#include "graphics/mesh.h"

#include <stdexcept>
#include <utility>

namespace gfx {

namespace {

/**
 * @brief Ensures the provided vertex positions, normals, texture coordinates, and element indices describe a
 *        triangle mesh in addition to enforcing alignment between vertex attribute.
 */
void Validate(const std::span<const glm::vec3> positions,
              const std::span<const glm::vec3> normals,
              const std::span<const glm::vec2> texcoords,
              const std::span<const GLuint> indices) {
  if (positions.empty()) {
    throw std::invalid_argument{"Vertex positions must be specified"};
  }
  if ((indices.empty() && positions.size() % 3 != 0) || indices.size() % 3 != 0) {
    throw std::invalid_argument{"Object must be a triangle mesh"};
  }
  if (indices.empty() && !texcoords.empty() && positions.size() != texcoords.size()) {
    throw std::invalid_argument{"Texture coordinates must align with position data"};
  }
  if (indices.empty() && !normals.empty() && positions.size() != normals.size()) {
    throw std::invalid_argument{"Vertex normals must align with position data"};
  }
}

}  // namespace

Mesh::Mesh(const std::span<const glm::vec3> positions,
           const std::span<const glm::vec3> normals,
           const std::span<const glm::vec2> texcoords,
           const std::span<const GLuint> indices,
           const glm::mat4& model_transform)
    : positions_{positions.begin(), positions.end()},
      normals_{normals.begin(), normals.end()},
      texcoords_{texcoords.begin(), texcoords.end()},
      indices_{indices.begin(), indices.end()},
      model_transform_{model_transform} {
  Validate(positions_, normals_, texcoords_, indices_);

  glGenVertexArrays(1, &vertex_array_);
  glBindVertexArray(vertex_array_);

  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

  using PositionType = decltype(positions_)::value_type;
  using NormalType = decltype(normals_)::value_type;
  using TextCoordsType = decltype(texcoords_)::value_type;

  // allocate memory for the vertex buffer
  const auto positions_size = static_cast<GLsizeiptr>(sizeof(PositionType) * positions_.size());
  const auto normals_size = static_cast<GLsizeiptr>(sizeof(NormalType) * normals_.size());
  const auto texcoords_size = static_cast<GLsizeiptr>(sizeof(TextCoordsType) * texcoords_.size());
  const auto buffer_size = positions_size + normals_size + texcoords_size;
  glNamedBufferStorage(vertex_buffer_, buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT);

  // copy positions to the vertex buffer
  static constexpr auto kPositionAttributeIndex = 0;
  static constexpr auto kPositionsOffset = 0;
  glNamedBufferSubData(vertex_buffer_, kPositionsOffset, positions_size, positions_.data());
  glVertexAttribPointer(kPositionAttributeIndex,
                        PositionType::length(),
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
                        reinterpret_cast<const void*>(kPositionsOffset));
  glEnableVertexAttribArray(kPositionAttributeIndex);

  // copy normals to the vertex buffer
  if (!normals_.empty()) {
    static constexpr auto kNormalAttributeIndex = 1;
    const auto normals_offset = positions_size;
    glNamedBufferSubData(vertex_buffer_, normals_offset, normals_size, normals_.data());
    glVertexAttribPointer(kNormalAttributeIndex,
                          NormalType::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
                          reinterpret_cast<const void*>(normals_offset));
    glEnableVertexAttribArray(kNormalAttributeIndex);
  }

  // copy texture coordinates to the vertex buffer
  if (!texcoords_.empty()) {
    static constexpr auto kTexCoordsAttributeIndex = 2;
    const auto texcoords_offset = positions_size + normals_size;
    glNamedBufferSubData(vertex_buffer_, texcoords_offset, texcoords_size, texcoords_.data());
    glVertexAttribPointer(kTexCoordsAttributeIndex,
                          TextCoordsType::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
                          reinterpret_cast<const void*>(texcoords_offset));
    glEnableVertexAttribArray(kTexCoordsAttributeIndex);
  }

  // copy indices to the element buffer
  if (!indices_.empty()) {
    using IndexType = decltype(indices_)::value_type;
    const auto indices_size = static_cast<GLsizeiptr>(sizeof(IndexType) * indices_.size());
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glNamedBufferStorage(element_buffer_, indices_size, indices_.data(), 0);
  }
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
  if (this != &mesh) {
    vertex_array_ = std::exchange(mesh.vertex_array_, 0);
    vertex_buffer_ = std::exchange(mesh.vertex_buffer_, 0);
    element_buffer_ = std::exchange(mesh.element_buffer_, 0);
    positions_ = std::move(mesh.positions_);
    normals_ = std::move(mesh.normals_);
    texcoords_ = std::move(mesh.texcoords_);
    indices_ = std::move(mesh.indices_);
    model_transform_ = std::exchange(mesh.model_transform_, glm::mat4{0.0f});
  }
  return *this;
}

Mesh::~Mesh() noexcept {
  glDeleteVertexArrays(1, &vertex_array_);
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &element_buffer_);
}

}  // namespace gfx
