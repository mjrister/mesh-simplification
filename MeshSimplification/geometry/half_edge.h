#pragma once

#include <format>
#include <memory>

#include "geometry/face.h"
#include "geometry/vertex.h"

namespace geometry {

	/** \brief A directional edge in a half-edge mesh. */
	class HalfEdge {

	public:
		/**
		 * \brief Initializes a half-edge.
		 * \param vertex The vertex the half-edge will point to.
		 */
		explicit HalfEdge(std::shared_ptr<Vertex> vertex) : vertex_{std::move(vertex)} {}

		/** \brief Gets the vertex at the head of this half-edge. */
		[[nodiscard]] std::shared_ptr<Vertex> Vertex() const { return vertex_; }

		/** \brief Gets the next half-edge of a triangle in counter-clockwise order. */
		[[nodiscard]] std::shared_ptr<HalfEdge> Next() const { return next_; }

		/** \brief Sets the next half-edge. */
		void SetNext(const std::shared_ptr<HalfEdge>& next) { next_ = next; }

		/** \brief Gets the half-edge that shares this edge's vertices in the opposite direction. */
		[[nodiscard]] std::shared_ptr<HalfEdge> Flip() const { return flip_; }

		/** \brief Sets the flip half-edge. */
		void SetFlip(const std::shared_ptr<HalfEdge>& flip) { flip_ = flip; }

		/** \brief Gets the face created by three counter-clockwise \c next iterations starting from this half-edge. */
		[[nodiscard]] std::shared_ptr<Face> Face() const { return face_; }

		/** Sets the half-edge face. */
		void SetFace(const std::shared_ptr<geometry::Face>& face) { face_ = face; }

		/** \brief Gets the half-edge hash value. */
		friend std::size_t hash_value(const HalfEdge& edge) { return hash_value(*edge.flip_->vertex_, *edge.vertex_); }

	private:
		const std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_, flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}

// defines an explicit specialization for use with std::format
template<>
struct std::formatter<geometry::HalfEdge> : std::formatter<std::string> {
	auto format(const geometry::HalfEdge& half_edge, std::format_context& context) {
		return formatter<std::string>::format(
			std::format("({},{})", *half_edge.Flip()->Vertex(), *half_edge.Vertex()), context);
	}
};
