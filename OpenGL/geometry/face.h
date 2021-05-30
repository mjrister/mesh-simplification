#pragma once

#include <array>
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

		static std::array<std::shared_ptr<Vertex>, 3> GetMinVertexOrder(
			const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) {

			if (const auto min_id = std::min<>({v0->Id(), v1->Id(), v2->Id()}); min_id == v0->Id()) {
				return {v0, v1, v2};
			} else if (min_id == v1->Id()) {
				return {v1, v2, v0};
			} else {
				return {v2, v0, v1};
			}
		}

		Face(const std::size_t id,
		     const std::shared_ptr<Vertex>& v0,
		     const std::shared_ptr<Vertex>& v1,
		     const std::shared_ptr<Vertex>& v2) : id_{id} {

			const auto min_vertex_order = GetMinVertexOrder(v0, v1, v2);
			v0_ = min_vertex_order[0];
			v1_ = min_vertex_order[1];
			v2_ = min_vertex_order[2];
		}

		[[nodiscard]] std::size_t Id() const { return id_; }
		[[nodiscard]] std::shared_ptr<Vertex> V0() const { return v0_; }
		[[nodiscard]] std::shared_ptr<Vertex> V1() const { return v1_; }
		[[nodiscard]] std::shared_ptr<Vertex> V2() const { return v2_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Edge() const { return edge_; }
		void SetEdge(const std::shared_ptr<HalfEdge>& edge) { edge_ = edge; }

	private:
		const std::size_t id_;
		std::shared_ptr<Vertex> v0_, v1_, v2_;
		std::shared_ptr<HalfEdge> edge_;
	};
}
