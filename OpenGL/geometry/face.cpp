#include "geometry/face.h"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <tuple>

#include <glm/glm.hpp>

namespace {
	auto GetMinVertexOrder(
		const std::shared_ptr<const geometry::Vertex>& v0,
		const std::shared_ptr<const geometry::Vertex>& v1,
		const std::shared_ptr<const geometry::Vertex>& v2) {

		if (const auto min_id = std::min<>({v0->Id(), v1->Id(), v2->Id()}); min_id == v0->Id()) {
			return std::make_tuple(v0, v1, v2);
		} else if (min_id == v1->Id()) {
			return std::make_tuple(v1, v2, v0);
		} else {
			return std::make_tuple(v2, v0, v1);
		}
	}

	glm::vec3 GetFaceNormal(const geometry::Vertex& v0, const geometry::Vertex& v1, const geometry::Vertex& v2) {
		const glm::vec3 edge01 = v1.Position() - v0.Position();
		const glm::vec3 edge02 = v2.Position() - v0.Position();
		const auto normal = glm::normalize(glm::cross(edge01, edge02));
		if (std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z)) {
			std::ostringstream oss;
			oss << '(' << v0 << ',' << v1 << ',' << v2 << ") is not a triangle";
			throw std::invalid_argument{oss.str()};
		}
		return normal;
	}
}

geometry::Face::Face(
	const std::shared_ptr<const Vertex>& v0,
	const std::shared_ptr<const Vertex>& v1,
	const std::shared_ptr<const Vertex>& v2) : normal_{GetFaceNormal(*v0, *v1, *v2)} {
	std::tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);
}
