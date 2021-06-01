#pragma once

#include <memory>
#include <ostream>

#include <glm/vec3.hpp>

namespace geometry {
	class HalfEdge;

	class Vertex {

	public:
		static Vertex Average(const std::size_t id, const Vertex& v0, const Vertex& v1) {
			const auto position = (v0.position_ + v1.position_) / 2.f;
			const auto normal = (v0.normal_ + v1.normal_) / 2.f;
			return Vertex{id, position, normal};
		}

		Vertex(const std::size_t id, const glm::vec3& position, const glm::vec3& normal)
			: id_{id}, position_{position}, normal_{normal} {}

		[[nodiscard]] std::size_t Id() const { return id_; }
		[[nodiscard]] const glm::vec3& Position() const { return position_; }
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

		friend bool operator==(const Vertex& lhs, const Vertex& rhs) {
			return lhs.id_ == rhs.id_ && lhs.position_ == rhs.position_ && lhs.normal_ == rhs.normal_;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
			return os << vertex.id_;
		}

	private:
		const std::size_t id_;
		const glm::vec3 position_, normal_;
		std::shared_ptr<HalfEdge> edge_;
	};
}
