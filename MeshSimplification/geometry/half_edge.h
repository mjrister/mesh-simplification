#pragma once

#include <memory>
#include <ostream>

#include "geometry/face.h"
#include "geometry/vertex.h"

namespace geometry {

	/** \brief A directional edge in a half-edge mesh. */
	class HalfEdge {

	public:
		/**
		 * \brief Initializes a half-edge.
		 * \param vertex The vertex this half-edge will point to.
		 */
		explicit HalfEdge(std::shared_ptr<Vertex> vertex) : vertex_{std::move(vertex)} {}

		/** \brief Gets the vertex at the head of this half-edge. */
		[[nodiscard]] std::shared_ptr<Vertex> Vertex() const { return vertex_; }

		/** \brief Gets the next edge in counter-clockwise order of the triangle this half-edge comprises. */
		[[nodiscard]] std::shared_ptr<HalfEdge> Next() const { return next_; }
		void SetNext(const std::shared_ptr<HalfEdge>& next) { next_ = next; }

		/** \brief Gets a directional half-edge that shares this edge's vertices in the opposite direction. */
		[[nodiscard]] std::shared_ptr<HalfEdge> Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<HalfEdge>& flip) { flip_ = flip; }

		/** \brief Gets the triangle created by performing three counter-clockwise \c next iterations. */
		[[nodiscard]] std::shared_ptr<Face> Face() const { return face_; }
		void SetFace(const std::shared_ptr<geometry::Face>& face) { face_ = face; }

		/** \brief Sends a string representation of a half-edge to an output stream. */
		friend std::ostream& operator<<(std::ostream& os, const HalfEdge& edge) {
			return os << '(' << *edge.flip_->vertex_ << ',' << *edge.vertex_ << ')';
		}

		/** \brief Gets the hash value for a half-edge. */
		friend std::size_t hash_value(const HalfEdge& edge) {
			return hash_value(*edge.flip_->vertex_, *edge.vertex_);
		}

	private:
		const std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_, flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}
