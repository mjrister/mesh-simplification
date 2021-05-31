#pragma once

#include <memory>
#include <ostream>

#include "vertex.h"

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

		friend bool operator==(const Face& lhs, const Face& rhs) { return lhs.id_ == rhs.id_; }
		friend bool operator!=(const Face& lhs, const Face& rhs) { return !(lhs == rhs); }

		friend std::ostream& operator<<(std::ostream& os, const Face& face) {
			return os << '(' << *face.v0_ << ',' << *face.v1_ << ',' << *face.v2_ << ')';
		}

	private:
		std::uint64_t id_;
		std::shared_ptr<Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}

namespace std {
	template <>
	struct hash<geometry::Face> {
		std::size_t operator()(const geometry::Face& face) const noexcept {
			std::uint64_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(face.V0()->Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(face.V1()->Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(face.V2()->Id());
			return seed;
		}
	};
}
