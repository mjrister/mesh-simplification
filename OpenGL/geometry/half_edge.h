#pragma once

#include <memory>
#include <ostream>
#include <utility>

#include "face.h"
#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		explicit HalfEdge(std::shared_ptr<Vertex> vertex)
			: vertex_{std::move(vertex)} {}

		[[nodiscard]] auto Vertex() const { return vertex_; }

		[[nodiscard]] auto Next() const { return next_; }
		void SetNext(const std::shared_ptr<const HalfEdge>& next) { next_ = next; }

		[[nodiscard]] auto Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<const HalfEdge>& flip) { flip_ = flip; }

		[[nodiscard]] auto Face() const { return face_; }
		void SetFace(const std::shared_ptr<const geometry::Face>& face) { face_ = face; }

		friend std::ostream& operator<<(std::ostream& os, const HalfEdge& half_edge) {
			return os << '(' << half_edge.vertex_ << ',' << half_edge.flip_->vertex_ << ')';
		}

	private:
		std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_;
		std::shared_ptr<HalfEdge> flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}
