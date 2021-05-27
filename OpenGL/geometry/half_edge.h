#pragma once

#include <memory>
#include <utility>

#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		HalfEdge(const std::uint32_t id, std::shared_ptr<const Vertex> head)
			: id_{id}, head_{std::move(head)} {}

		[[nodiscard]] auto Id() const { return id_; }
		[[nodiscard]] const auto& Head() const { return head_; }

		[[nodiscard]] std::shared_ptr<const HalfEdge> Next() const { return next_; }
		void SetNext(const std::shared_ptr<const HalfEdge>& next) { next_ = next; }

		[[nodiscard]] std::shared_ptr<const HalfEdge> Flip() const { return flip_; }
		void SetFlip(const std::shared_ptr<const HalfEdge>& flip) { flip_ = flip; }

	private:
		const std::uint32_t id_;
		const std::shared_ptr<const Vertex> head_;
		std::shared_ptr<const HalfEdge> next_;
		std::shared_ptr<const HalfEdge> flip_;
	};
}
