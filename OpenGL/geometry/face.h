#pragma once

#include <memory>
#include <ostream>
#include <utility>

#include "vertex.h"

namespace geometry {
	class HalfEdge;

	class Face {

	public:
		Face(std::shared_ptr<Vertex> v0, std::shared_ptr<Vertex> v1, std::shared_ptr<Vertex> v2)
			: v0_{std::move(v0)}, v1_{std::move(v1)}, v2_{std::move(v2)} {}

		[[nodiscard]] auto V0() const { return v0_; }
		[[nodiscard]] auto V1() const { return v1_; }
		[[nodiscard]] auto V2() const { return v2_; }

		[[nodiscard]] auto Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<const HalfEdge>& edge) { edge_ = edge; }

		friend std::ostream& operator<<(std::ostream& os, const Face& face) {
			return os << '(' << face.v0_ << ',' << face.v1_ << ',' << face.v2_ << ')';
		}

	private:
		std::shared_ptr<Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}