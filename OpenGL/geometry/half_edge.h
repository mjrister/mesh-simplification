#pragma once

#include <memory>
#include <utility>

#include "face.h"
#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		explicit HalfEdge(std::shared_ptr<const Vertex> vertex)
			: vertex_{std::move(vertex)} {}

		[[nodiscard]] auto Vertex() const { return vertex_; }

		[[nodiscard]] auto Next() const { return next_; }
		void SetNext(const std::shared_ptr<const HalfEdge>& next) { next_ = next; }

		[[nodiscard]] auto Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<const HalfEdge>& flip) { flip_ = flip; }

		[[nodiscard]] auto Face() const { return face_; }
		void SetFace(const std::shared_ptr<const geometry::Face>& face) { face_ = face; }

	private:
		const std::shared_ptr<const geometry::Vertex> vertex_;
		std::shared_ptr<const HalfEdge> next_;
		std::shared_ptr<const HalfEdge> flip_;
		std::shared_ptr<const geometry::Face> face_;
	};
}
