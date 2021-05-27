#pragma once

#include <memory>

#include "half_edge.h"
#include "vertex.h"

namespace geometry {

	class Face {

	public:
		Face(std::shared_ptr<const Vertex> v0, std::shared_ptr<const Vertex> v1, std::shared_ptr<const Vertex> v2)
			: v0_{std::move(v0)}, v1_{std::move(v1)}, v2_{std::move(v2)} {}

		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

	private:
		const std::shared_ptr<const Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}
