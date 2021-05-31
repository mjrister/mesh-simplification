#pragma once

#include <memory>
#include <ostream>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace geometry {
	class HalfEdge;

	class Vertex {

	public:
		Vertex(const std::uint64_t id, const glm::vec4& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		[[nodiscard]] std::uint64_t Id() const { return id_; }
		[[nodiscard]] const glm::vec4& Position() const { return position_; }
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

		friend bool operator==(const Vertex& lhs, const Vertex& rhs) { return lhs.id_ == rhs.id_; }
		friend bool operator!=(const Vertex& lhs, const Vertex& rhs) { return !(lhs == rhs); }

		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) { return os << vertex.id_; }

	private:
		const std::uint64_t id_;
		const glm::vec4 position_;
		const glm::vec3 normal_;
		std::shared_ptr<HalfEdge> edge_;
	};
}