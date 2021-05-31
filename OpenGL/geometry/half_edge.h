#pragma once

#include <memory>
#include <ostream>
#include <utility>

#include "face.h"
#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		static std::uint64_t GetHalfEdgeId(const Vertex& v0, const Vertex& v1) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
			return seed;
		}

		explicit HalfEdge(const std::uint64_t id, std::shared_ptr<Vertex> vertex)
			: id_{id}, vertex_{std::move(vertex)} {}

		[[nodiscard]] std::uint64_t Id() const { return id_; }
		[[nodiscard]] std::shared_ptr<Vertex> Vertex() const { return vertex_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Next() const { return next_; }
		void SetNext(const std::shared_ptr<HalfEdge>& next) { next_ = next; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<HalfEdge>& flip) { flip_ = flip; }

		[[nodiscard]] std::shared_ptr<Face> Face() const { return face_; }
		void SetFace(const std::shared_ptr<geometry::Face>& face) { face_ = face; }

		friend std::ostream& operator<<(std::ostream& os, const HalfEdge& edge) {
			return os << '(' << *edge.vertex_ << ',' << *edge.flip_->vertex_ << ')';
		}

	private:
		const std::uint64_t id_;
		const std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_, flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}
