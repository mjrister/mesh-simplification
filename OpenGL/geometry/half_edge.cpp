#include "half_edge.h"

namespace {
	std::uint64_t GetHalfEdgeId(const geometry::Vertex& v0, const geometry::Vertex& v1) {
		std::size_t seed = 0x1C2CB417;
		seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
		seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
		return seed;
	}
}

geometry::HalfEdge::HalfEdge(
	const std::shared_ptr<geometry::Vertex>& v0, const std::shared_ptr<geometry::Vertex>& v1)
	: id_{GetHalfEdgeId(*v0, *v1)},
	  vertex_{v1} {}

std::ostream& geometry::operator<<(std::ostream& os, const HalfEdge& edge) {
	return os << '(' << *edge.vertex_ << ',' << *edge.flip_->vertex_ << ')';
}
