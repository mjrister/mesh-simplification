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
	explicit HalfEdge(std::shared_ptr<Vertex> vertex) noexcept : vertex_{std::move(vertex)} {}

	/** \brief Gets the vertex at the head of this half-edge. */
	[[nodiscard]] std::shared_ptr<Vertex> vertex() const noexcept { return vertex_; }

	/** \brief Gets the next half-edge of a triangle in counter-clockwise order. */
	[[nodiscard]] std::shared_ptr<HalfEdge> next() const noexcept { return next_; }

	/** \brief Sets the next half-edge. */
	void set_next(const std::shared_ptr<HalfEdge>& next) noexcept { next_ = next; }

	/** \brief Gets the half-edge that shares this edge's vertices in the opposite direction. */
	[[nodiscard]] std::shared_ptr<HalfEdge> flip() const noexcept { return flip_; }

	/** \brief Sets the flip half-edge. */
	void set_flip(const std::shared_ptr<HalfEdge>& flip) noexcept { flip_ = flip; }

	/** \brief Gets the face created by three counter-clockwise \c next iterations starting from this half-edge. */
	[[nodiscard]] std::shared_ptr<Face> face() const noexcept { return face_; }

	/** Sets the half-edge face. */
	void set_face(const std::shared_ptr<Face>& face) noexcept { face_ = face; }

	/** \brief Gets the half-edge hash value. */
	friend std::size_t hash_value(const HalfEdge& edge) noexcept { return hash_value(*edge.flip_->vertex_, *edge.vertex_); }

private:
	std::shared_ptr<Vertex> vertex_;
	std::shared_ptr<HalfEdge> next_, flip_;
	std::shared_ptr<Face> face_;
};
}

// defines an explicit specialization for use with std::format
template <>
struct std::formatter<geometry::HalfEdge> : std::formatter<std::string> {
	auto format(const geometry::HalfEdge& half_edge, std::format_context& context) {
		return formatter<std::string>::format(
			std::format("({},{})", *half_edge.flip()->vertex(), *half_edge.vertex()), context);
	}
};
