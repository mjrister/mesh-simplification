#pragma once

#include <map>
#include <memory>
#include <unordered_map>

namespace gfx {
	class Mesh;
}

namespace geometry {
	class HalfEdge;
	class Face;
	class Vertex;

	class HalfEdgeMesh {

	public:
		explicit HalfEdgeMesh(const gfx::Mesh& mesh);

		[[nodiscard]] const auto& Vertices() const { return vertices_; }
		[[nodiscard]] const auto& Edges() const { return edges_; }
		[[nodiscard]] const auto& Faces() const { return faces_; }

		operator gfx::Mesh() const;

		void CollapseEdge(
			const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v_new);

	private:
		std::map<std::size_t, std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
	};
}
