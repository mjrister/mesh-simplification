#include "face.h"

#include <algorithm>
#include <sstream>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

using namespace geometry;

namespace {
	std::uint64_t GetFaceId(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		std::uint64_t seed = 0x1C2CB417;
		seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
		seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
		seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(v2.Id());
		return seed;
	}

	auto GetMinVertexOrder(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		const std::shared_ptr<Vertex>& v2) {

		if (const auto min_id = std::min<>({v0->Id(), v1->Id(), v2->Id()}); min_id == v0->Id()) {
			return make_tuple(v0, v1, v2);
		} else if (min_id == v1->Id()) {
			return make_tuple(v1, v2, v0);
		} else {
			return make_tuple(v2, v0, v1);
		}
	}

	bool IsTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		const glm::vec3 edge01 = v1.Position() - v0.Position();
		const glm::vec3 edge02 = v2.Position() - v0.Position();
		return cross(edge01, edge02) != glm::vec3{0.f};
	}
}

Face::Face(
	const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) {

	std::tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);
	id_ = GetFaceId(*v0_, *v1_, *v2_);

	if (!IsTriangle(*v0_, *v1_, *v2_)) {
		std::ostringstream oss;
		oss << *this << " is not a triangle";
		throw std::invalid_argument{oss.str()};
	}
}
