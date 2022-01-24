#pragma once

#include <format>
#include <memory>

#include <glm/vec3.hpp>

namespace geometry {
class HalfEdge;

/** \brief A half-edge mesh vertex. */
class Vertex {

public:
	/**
	 * \brief Initializes a vertex.
	 * \param id The vertex ID.
	 * \param position The vertex position.
	 */
	Vertex(const std::size_t id, const glm::vec3& position) noexcept : id_{id}, position_{position} {}

	/** \brief Gets the vertex ID. */
	[[nodiscard]] std::size_t Id() const noexcept { return id_; }

	/** \brief Gets the vertex position. */
	[[nodiscard]] const glm::vec3& Position() const noexcept { return position_; }

	/** \brief Gets the last created half-edge that points to this vertex. */
	[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const noexcept { return edge_; }

	/** \brief Sets the vertex half-edge. */
	void SetEdge(const std::shared_ptr<HalfEdge>& edge) noexcept { edge_ = edge; }

	/** \brief Gets the hash value for a vertex. */
	friend std::size_t hash_value(const Vertex& v0) noexcept { return std::hash<std::size_t>{}(v0.id_); }

	/** \brief Gets the hash value for two vertices. */
	friend std::size_t hash_value(const Vertex& v0, const Vertex& v1) noexcept {
		std::size_t seed = 0x32C95994;
		seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + hash_value(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + hash_value(v1);
		return seed;
	}

	/** \brief Gets the hash value for three vertices. */
	friend std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) noexcept {
		std::size_t seed = 0x230402B5;
		seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + hash_value(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + hash_value(v1);
		seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + hash_value(v2);
		return seed;
	}

private:
	std::size_t id_;
	glm::vec3 position_;
	std::shared_ptr<HalfEdge> edge_;
};
}

// defines an explicit specialization for use with std::format
template <>
struct std::formatter<geometry::Vertex> : std::formatter<std::string> {
	auto format(const geometry::Vertex& vertex, std::format_context& context) {
		return formatter<std::string>::format(to_string(vertex.Id()), context);
	}
};
