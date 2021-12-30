#include "geometry/vertex.h"

#include <glm/glm.hpp>

#include <geometry/half_edge.h>

using namespace geometry;
using namespace glm;

vec3 Vertex::Normal() const {
	vec3 normal{0.f};
	uint8_t face_count = 0;
	auto edgei0 = edge_;
	do {
		normal += edgei0->Face()->Normal();
		edgei0 = edgei0->Next()->Flip();
		++face_count;
	} while (edgei0 != edge_);
	return normalize(normal / static_cast<float>(face_count));
}
