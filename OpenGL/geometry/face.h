#pragma once

#include <memory>
#include <ostream>

#include "geometry/vertex.h"

namespace geometry {
	class HalfEdge;

	/** \brief A triangle face defined by three vertices in counter-clockwise winding order. */
	class Face {

	public:
		/**
		 * \brief Initializes a triangle face.
		 * \param v0,v1,v2 The face vertices.
		 */
		Face(const std::shared_ptr<const Vertex>& v0,
		     const std::shared_ptr<const Vertex>& v1,
		     const std::shared_ptr<const Vertex>& v2);

		/** \brief Gets the first face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> V0() const { return v0_; }

		/** \brief  Gets the second face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> V1() const { return v1_; }

		/** \brief Gets the third face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> V2() const { return v2_; }

		/** \brief  Gets the face normal. */
		[[nodiscard]] const glm::vec3& Normal() const { return normal_; }

		/** \brief Sends a string representation of a face to an output stream. */
		friend std::ostream& operator<<(std::ostream& os, const Face& face) {
			return os << '(' << *face.v0_ << ',' << *face.v1_ << ',' << *face.v2_ << ')';
		}

		/** \brief Gets the face hash value. */
		friend std::size_t hash_value(const Face& face) {
			return hash_value(*face.v0_, *face.v1_, *face.v2_);
		}

	private:
		std::shared_ptr<const Vertex> v0_, v1_, v2_;
		glm::vec3 normal_;
	};
}
