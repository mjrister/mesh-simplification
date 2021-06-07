#pragma once

#include <memory>
#include <ostream>

#include "geometry/vertex.h"

namespace geometry {
	class HalfEdge;

	class Face {

	public:
		Face(const std::shared_ptr<const Vertex>& v0,
		     const std::shared_ptr<const Vertex>& v1,
		     const std::shared_ptr<const Vertex>& v2);

		[[nodiscard]] std::shared_ptr<const Vertex> V0() const { return v0_; }
		[[nodiscard]] std::shared_ptr<const Vertex> V1() const { return v1_; }
		[[nodiscard]] std::shared_ptr<const Vertex> V2() const { return v2_; }
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		friend std::ostream& operator<<(std::ostream& os, const Face& face) {
			return os << '(' << *face.v0_ << ',' << *face.v1_ << ',' << *face.v2_ << ')';
		}

		friend std::size_t hash_value(const Face& face) {
			return hash_value(*face.v0_, *face.v1_, *face.v2_);
		}

	private:
		std::shared_ptr<const Vertex> v0_, v1_, v2_;
		glm::vec3 normal_;
	};
}
