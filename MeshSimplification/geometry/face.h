#pragma once

#include <format>
#include <memory>

#include <glm/vec3.hpp>

#include "geometry/vertex.h"

namespace qem {
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
	[[nodiscard]] std::shared_ptr<const Vertex> V0() const { return std::shared_ptr{v0_}; }

	/** \brief Gets the second face vertex. */
	[[nodiscard]] std::shared_ptr<const Vertex> V1() const { return std::shared_ptr{v1_}; }

	/** \brief Gets the third face vertex. */
	[[nodiscard]] std::shared_ptr<const Vertex> V2() const { return std::shared_ptr{v2_}; }

	/** \brief Gets the face normal. */
	[[nodiscard]] const glm::vec3& Normal() const noexcept { return normal_; }

	/** \brief Gets the face area. */
	[[nodiscard]] float Area() const noexcept { return area_; }

	/** \brief Gets the face hash value. */
	friend std::uint64_t hash_value(const Face& face) { return hash_value(*face.V0(), *face.V1(), *face.V2()); }

private:
	std::weak_ptr<const Vertex> v0_, v1_, v2_;
	glm::vec3 normal_;
	float area_;
};
}

// defines an explicit specialization for use with std::format
template <>
struct std::formatter<qem::Face> : std::formatter<string> {
	auto format(const qem::Face& face, format_context& context) {
		return std::formatter<string>::format(std::format("({},{},{})", *face.V0(), *face.V1(), *face.V2()), context);
	}
};
