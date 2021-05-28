#pragma once

#include <memory>

#include "vertex.h"

namespace geometry {
	class HalfEdge;

	class Face {

	public:
		Face(std::shared_ptr<const Vertex> v0, std::shared_ptr<const Vertex> v1, std::shared_ptr<const Vertex> v2)
			: v0_{std::move(v0)},
			  v1_{std::move(v1)},
			  v2_{std::move(v2)} {}

		[[nodiscard]] std::shared_ptr<const Vertex> V0() const { return v0_; }
		[[nodiscard]] std::shared_ptr<const Vertex> V1() const { return v1_; }
		[[nodiscard]] std::shared_ptr<const Vertex> V2() const { return v2_; }

		[[nodiscard]] std::shared_ptr<const HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<const HalfEdge>& edge) { edge_ = edge; }

	private:
		const std::shared_ptr<const Vertex> v0_, v1_, v2_;
		std::shared_ptr<const HalfEdge> edge_;
	};
}