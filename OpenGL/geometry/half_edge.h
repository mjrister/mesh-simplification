#pragma once

#include <memory>
#include <utility>

#include "face.h"
#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		explicit HalfEdge(std::shared_ptr<Vertex> vertex)
			: vertex_{std::move(vertex)} {}

		[[nodiscard]] std::shared_ptr<Vertex> Vertex() const { return vertex_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Next() const { return next_; }
		void SetNext(const std::shared_ptr<HalfEdge>& next) { next_ = next; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<HalfEdge>& flip) { flip_ = flip; }

		[[nodiscard]] std::shared_ptr<Face> Face() const { return face_; }
		void SetFace(const std::shared_ptr<geometry::Face>& face) { face_ = face; }

	private:
		std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_;
		std::shared_ptr<HalfEdge> flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}
