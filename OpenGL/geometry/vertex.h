#pragma once

#include <memory>

#include <glm/vec3.hpp>

namespace geometry {
	class HalfEdge;

	class Vertex {

	public:
		Vertex(const std::uint64_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id},
		      position_{position},
			  normal_{normal} {}

		[[nodiscard]] glm::vec3 Position() const { return position_; }
		[[nodiscard]] glm::vec3 Normal() const { return normal_; }

		[[nodiscard]] std::shared_ptr<const HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<const HalfEdge>& edge) { edge_ = edge; }

		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.id_);
			return seed;
		}

		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(v2.id_);
			return seed;
		}

	private:
		std::uint64_t id_;
		const glm::vec3 position_, normal_;
		std::shared_ptr<const HalfEdge> edge_;
	};
}