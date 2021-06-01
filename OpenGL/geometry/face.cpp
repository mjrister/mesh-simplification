#include "geometry/face.h"

#include <algorithm>
#include <array>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

using namespace geometry;

namespace {
	std::array<std::shared_ptr<Vertex>, 3> GetMinVertexOrder(
		const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) {

		if (const auto min_id = std::min<>({v0->Id(), v1->Id(), v2->Id()}); min_id == v0->Id()) {
			return {v0, v1, v2};
		} else if (min_id == v1->Id()) {
			return {v1, v2, v0};
		} else {
			return {v2, v0, v1};
		}
	}

	bool IsCollinear(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		const glm::vec3 edge01 = v1.Position() - v0.Position();
		const glm::vec3 edge02 = v2.Position() - v0.Position();
		return glm::cross(edge01, edge02) == glm::vec3{0.f};
	}
}

Face::Face(
	const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) {

	const auto vertex_order = GetMinVertexOrder(v0, v1, v2);
	v0_ = vertex_order[0];
	v1_ = vertex_order[1];
	v2_ = vertex_order[2];
	id_ = hash_value(*v0_, *v1_, *v2_);

	if (IsCollinear(*v0_, *v1_, *v2_)) {
		std::ostringstream oss;
		oss << *this << " is not a triangle";
		throw std::invalid_argument{oss.str()};
	}
}
