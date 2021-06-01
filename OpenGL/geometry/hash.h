#pragma once

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"

namespace geometry {

	inline std::size_t hash_value(const Vertex& v0) {
		std::size_t seed = 0x3E9EB221;
		seed ^= (seed << 6) + (seed >> 2) + 0x3573AC13 + std::hash<std::uint64_t>{}(v0.Id());
		return seed;
	}

	inline std::size_t hash_value(const Vertex& v0, const Vertex& v1) {
		std::size_t seed = 0x32C95994;
		seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + hash_value(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + hash_value(v1);
		return seed;
	}

	inline std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		std::size_t seed = 0x230402B5;
		seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + hash_value(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + hash_value(v1);
		seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + hash_value(v2);
		return seed;
	}

	inline std::size_t hash_value(const HalfEdge& edge) {
		return hash_value(*edge.Flip()->Vertex(), *edge.Vertex());
	}

	inline std::size_t hash_value(const Face& face) {
		return hash_value(*face.V0(), *face.V1(), *face.V2());
	}
}