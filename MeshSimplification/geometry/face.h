#pragma once

#include <format>
#include <memory>

#include <glm/vec3.hpp>

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
	[[nodiscard]] std::shared_ptr<const Vertex> v0() const noexcept { return v0_; }

	/** \brief  Gets the second face vertex. */
	[[nodiscard]] std::shared_ptr<const Vertex> v1() const noexcept { return v1_; }

	/** \brief Gets the third face vertex. */
	[[nodiscard]] std::shared_ptr<const Vertex> v2() const noexcept { return v2_; }

	/** \brief  Gets the face normal. */
	[[nodiscard]] const glm::vec3& normal() const noexcept { return normal_; }

	/** \brief Gets the face area. */
	[[nodiscard]] float area() const noexcept { return area_; }

	/** \brief Gets the face hash value. */
	friend std::size_t hash_value(const Face& face) noexcept { return hash_value(*face.v0_, *face.v1_, *face.v2_); }

private:
	std::shared_ptr<const Vertex> v0_, v1_, v2_;
	glm::vec3 normal_;
	float area_;
};
}

// defines an explicit specialization for use with std::format
template <>
struct std::formatter<geometry::Face> : std::formatter<std::string> {
	auto format(const geometry::Face& face, std::format_context& context) {
		return formatter<std::string>::format(
			std::format("({},{},{})", *face.v0(), *face.v1(), *face.v2()), context);
	}
};
