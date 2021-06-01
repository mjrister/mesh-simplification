#pragma once

#include <memory>
#include <ostream>

#include "geometry/vertex.h"

namespace geometry {
	class HalfEdge;

	class Face {

	public:
		Face(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2);

		[[nodiscard]] std::uint64_t Id() const { return id_; }
		[[nodiscard]] std::shared_ptr<Vertex> V0() const { return v0_; }
		[[nodiscard]] std::shared_ptr<Vertex> V1() const { return v1_; }
		[[nodiscard]] std::shared_ptr<Vertex> V2() const { return v2_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

		friend bool operator==(const Face& lhs, const Face& rhs) {
			return lhs.id_ == rhs.id_ && lhs.v0_ == rhs.v0_ && lhs.v1_ == rhs.v1_ && lhs.v2_ == rhs.v2_;
		}

		friend std::ostream& operator<<(std::ostream& os, const Face& face) {
			return os << '(' << *face.v0_ << ',' << *face.v1_ << ',' << *face.v2_ << ')';
		}

	private:
		std::uint64_t id_;
		std::shared_ptr<Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}
