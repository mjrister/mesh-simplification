#pragma once

#include <memory>

#include <glm/vec3.hpp>

namespace geometry {
	class HalfEdge;

	class Vertex {

	public:
		Vertex(const glm::vec3& position, const glm::vec3& normal)
			: position_{position},
			  normal_{normal} {}

		[[nodiscard]] glm::vec3 Position() const { return position_; }
		[[nodiscard]] glm::vec3 Normal() const { return normal_; }

		[[nodiscard]] std::shared_ptr<const HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<const HalfEdge>& edge) { edge_ = edge; }

	private:
		const glm::vec3 position_, normal_;
		std::shared_ptr<const HalfEdge> edge_;
	};
}