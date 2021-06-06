#pragma once

namespace gfx {
	class Mesh;
}

namespace geometry::mesh {
	gfx::Mesh Simplify(const gfx::Mesh& mesh, float stop_ratio);
}
