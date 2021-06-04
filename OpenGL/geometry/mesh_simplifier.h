#pragma once

namespace geometry {
	class HalfEdgeMesh;

	namespace mesh_simplifier {
		void Simplify(HalfEdgeMesh& mesh, float stop_ratio);
	}
}
