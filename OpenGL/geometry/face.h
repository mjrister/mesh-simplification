#pragma once

#include <memory>
#include <utility>

#include "vertex.h"

namespace geometry {
	class HalfEdge;

	class Face {

	public:
		static std::size_t GetFaceId(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(v2.Id());
			return seed;
		}

		Face(const std::size_t id, std::shared_ptr<Vertex> v0, std::shared_ptr<Vertex> v1, std::shared_ptr<Vertex> v2)
			: id_{id},
			  v0_{std::move(v0)},
			  v1_{std::move(v1)},
			  v2_{std::move(v2)} {}

		[[nodiscard]] std::size_t Id() const { return id_; }
		[[nodiscard]] std::shared_ptr<Vertex> V0() const { return v0_; }
		[[nodiscard]] std::shared_ptr<Vertex> V1() const { return v1_; }
		[[nodiscard]] std::shared_ptr<Vertex> V2() const { return v2_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

	private:
		const std::size_t id_;
		const std::shared_ptr<Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}