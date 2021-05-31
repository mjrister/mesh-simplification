#pragma once

#include <memory>
#include <ostream>

#include "face.h"
#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		static std::uint64_t GetHalfEdgeId(const Vertex& v0, const Vertex& v1);

		HalfEdge(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1)
			: id_{GetHalfEdgeId(*v0, *v1)}, vertex_{v1} {}

		[[nodiscard]] std::uint64_t Id() const { return id_; }
		[[nodiscard]] std::shared_ptr<Vertex> Vertex() const { return vertex_; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Next() const { return next_; }
		void SetNext(const std::shared_ptr<HalfEdge>& next) { next_ = next; }

		[[nodiscard]] std::shared_ptr<HalfEdge> Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<HalfEdge>& flip) { flip_ = flip; }

		[[nodiscard]] std::shared_ptr<Face> Face() const { return face_; }
		void SetFace(const std::shared_ptr<geometry::Face>& face) { face_ = face; }

		friend std::ostream& operator<<(std::ostream& os, const HalfEdge& edge);

	private:
		const std::uint64_t id_;
		const std::shared_ptr<geometry::Vertex> vertex_;
		std::shared_ptr<HalfEdge> next_, flip_;
		std::shared_ptr<geometry::Face> face_;
	};
}
