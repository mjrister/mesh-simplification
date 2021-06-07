#pragma once

#include <memory>
#include <ostream>

#include <glm/vec3.hpp>

namespace geometry {
	class HalfEdge;

	/** \brief A vertex in a half-edge mesh. */
	class Vertex {

	public:
		/**
		 * \brief Initializes a vertex.
		 * \param id The unique vertex ID.
		 * \param position The vertex position.
		 * \param normal The vertex normal.
		 */
		Vertex(const std::size_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		/** \brief Gets the vertex ID. */
		[[nodiscard]] std::size_t Id() const { return id_; }

		/** \brief Gets the vertex position. */
		[[nodiscard]] const glm::vec3& Position() const { return position_; }

		/** \brief Gets the vertex normal .*/
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		/** \brief Gets a half-edge that points to this vertex. */
		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

		/** \brief Sends the string representation of a vertex to an output stream. */
		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) { return os << vertex.id_; }

		/** \brief Gets the hash value for a vertex. */
		friend std::size_t hash_value(const Vertex& v0) { return std::hash<std::size_t>{}(v0.id_); }

		/** \brief Gets the hash value for two vertices. */
		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1) {
			std::size_t seed = 0x32C95994;
			seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + hash_value(v0);
			seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + hash_value(v1);
			return seed;
		}

		/** \brief Gets the hash value for three vertices. */
		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
			std::size_t seed = 0x230402B5;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + hash_value(v0);
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + hash_value(v1);
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + hash_value(v2);
			return seed;
		}

	private:
		const std::size_t id_;
		const glm::vec3 position_, normal_;
		std::shared_ptr<HalfEdge> edge_;
	};
}
