#pragma once

#include <memory>
#include <utility>

#include "vertex.h"

namespace geometry {

	class HalfEdge {

	public:
		HalfEdge(const std::uint32_t id, std::shared_ptr<Vertex> head)
			: id_{id},
			  head_{std::move(head)} {}

		[[nodiscard]] auto Id() const { return id_; }
		[[nodiscard]] const auto& Head() const { return head_; }

	private:
		std::uint32_t id_;
		std::shared_ptr<Vertex> head_;
	};
}
