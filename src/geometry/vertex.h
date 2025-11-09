#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>

#include <glm/vec3.hpp>

namespace gfx {
class HalfEdge;

/** @brief A half-edge mesh vertex. */
class Vertex {
public:
  /**
   * @brief Creates a vertex.
   * @param position The vertex position.
   */
  explicit Vertex(const glm::vec3& position) noexcept : position_{position} {}

  /**
   * @brief Initializes a vertex.
   * @param id The vertex ID.
   * @param position The vertex position.
   */
  Vertex(const int id, const glm::vec3& position) noexcept : id_{id}, position_{position} {}

  /** @brief Gets the vertex ID. */
  [[nodiscard]] int id() const noexcept {
    assert(id_.has_value());
    return *id_;
  }

  /** @brief Sets the vertex ID. */
  void set_id(const int id) noexcept { id_ = id; }

  /** @brief Gets the vertex position. */
  [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }

  /** @brief Gets the last created half-edge that points to this vertex. */
  [[nodiscard]] std::shared_ptr<const HalfEdge> edge() const noexcept {
    assert(!edge_.expired());
    return edge_.lock();
  }

  /** @brief Sets the vertex half-edge. */
  void set_edge(const std::shared_ptr<const HalfEdge>& edge) noexcept;

  /** @brief Defines the vertex equality operator. */
  friend bool operator==(const Vertex& lhs, const Vertex& rhs) noexcept { return lhs.id() == rhs.id(); }

  /** @brief Gets the hash value for a vertex. */
  friend std::size_t hash_value(const Vertex& v0) noexcept {
    static constexpr std::hash<int> kVertexIdHash;
    return kVertexIdHash(v0.id());
  }

  /** @brief Gets the hash value for a vertex pair. */
  friend std::size_t hash_value(const Vertex& v0, const Vertex& v1) noexcept {
    // NOLINTBEGIN(*-magic-numbers)
    std::size_t seed = 0x32C95994;
    seed ^= (seed << 6u) + (seed >> 2u) + 0x3FA612CEu + hash_value(v0);
    seed ^= (seed << 6u) + (seed >> 2u) + 0x197685C2u + hash_value(v1);
    // NOLINTEND(*-magic-numbers)
    return seed;
  }

  /** @brief Gets the hash value for vertex triple. */
  friend std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) noexcept {
    // NOLINTBEGIN(*-magic-numbers)
    std::size_t seed = 0x230402B5;
    seed ^= (seed << 6u) + (seed >> 2u) + 0x72C2C6EBu + hash_value(v0);
    seed ^= (seed << 6u) + (seed >> 2u) + 0x16E199E4u + hash_value(v1);
    seed ^= (seed << 6u) + (seed >> 2u) + 0x6F89F2A8u + hash_value(v2);
    // NOLINTEND(*-magic-numbers)
    return seed;
  }

private:
  std::optional<int> id_;
  glm::vec3 position_;
  std::weak_ptr<const HalfEdge> edge_;
};

}  // namespace gfx
