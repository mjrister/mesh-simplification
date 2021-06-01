#pragma once

#include <memory>
#include <ostream>

#include <glm/vec3.hpp>

namespace geometry {
	class HalfEdge;

	class Vertex {

	public:
		Vertex(const std::uint64_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		[[nodiscard]] std::uint64_t Id() const { return id_; }
		[[nodiscard]] const glm::vec3& Position() const { return position_; }
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

		friend bool operator==(const Vertex& lhs, const Vertex& rhs) {
			return lhs.id_ == rhs.id_ && lhs.position_ == rhs.position_ && lhs.normal_ == rhs.normal_;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) { return os << vertex.id_; }

		friend std::size_t hash_value(const Vertex& v0) {
			std::size_t seed = 0x3E9EB221;
			seed ^= (seed << 6) + (seed >> 2) + 0x3573AC13 + std::hash<std::uint64_t>{}(v0.id_);
			return seed;
		}

		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1) {
			std::size_t seed = 0x32C95994;
			seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + std::hash<std::uint64_t>{}(v0.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + std::hash<std::uint64_t>{}(v1.id_);
			return seed;
		}

		friend std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
			std::size_t seed = 0x230402B5;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.id_);
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(v2.id_);
			return seed;
		}

	private:
		const std::uint64_t id_;
		const glm::vec3 position_, normal_;
		std::shared_ptr<HalfEdge> edge_;
	};
}