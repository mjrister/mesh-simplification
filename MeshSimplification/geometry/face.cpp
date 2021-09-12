#include "geometry/face.h"

#include <algorithm>
#include <tuple>

#include <glm/geometric.hpp>

using namespace geometry;
using namespace glm;
using namespace std;

namespace {

	/**
	 * \brief Gets a canonical ordering of face vertices such that the vertex with the lowest ID.
	 * \param v0,v1,v2 The face vertices.
	 * \return A tuple consisting of \p v0, \p v1, \p v2 ordered by the lowest ID.
	 * \note Preserves winding order. The is necessary to disambiguate equivalent face elements queries.
	 */
	auto GetMinVertexOrder(
		const shared_ptr<const Vertex>& v0, const shared_ptr<const Vertex>& v1, const shared_ptr<const Vertex>& v2) {

		if (const auto min_id = min<>({v0->Id(), v1->Id(), v2->Id()}); min_id == v0->Id()) {
			return make_tuple(v0, v1, v2);
		} else if (min_id == v1->Id()) {
			return make_tuple(v1, v2, v0);
		} else {
			return make_tuple(v2, v0, v1);
		}
	}

	/**
	 * \brief Gets the face normal.
	 * \param v0,v1,v2 The face vertices.
	 * \return The 3D vector representing the face normal.
	 * \throw std::invalid_argument Indicates the face vertices do not represent a triangle. This can happen if \p v0,
	 *        \p v1, \p v2 are collinear or contains duplicates.
	 */
	vec3 GetFaceNormal(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		const vec3 edge01 = v1.Position() - v0.Position();
		const vec3 edge02 = v2.Position() - v0.Position();
		const auto normal = normalize(cross(edge01, edge02));
		if (any(isnan(normal))) {
			throw std::invalid_argument{format("({},{},{}) is not a triangle", v0, v1, v2)};
		}
		return normal;
	}
}

Face::Face(
	const shared_ptr<const Vertex>& v0,
	const shared_ptr<const Vertex>& v1,
	const shared_ptr<const Vertex>& v2) : normal_{GetFaceNormal(*v0, *v1, *v2)} {
	tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);
}
