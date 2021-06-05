#pragma once

namespace gfx {
	class Mesh;
}

namespace geometry::mesh_simplifier {
	gfx::Mesh Simplify(const gfx::Mesh& mesh, float stop_ratio);
}
