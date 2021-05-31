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
		gfx::Mesh ToMesh();

		[[nodiscard]] const auto& VerticesById() const { return vertices_by_id_; }
		[[nodiscard]] const auto& EdgesById() const { return edges_by_id_; }
		[[nodiscard]] const auto& FacesById() const { return faces_by_id_; }

	private:
		std::map<std::size_t, std::shared_ptr<Vertex>> vertices_by_id_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_by_id_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_by_id_;
	};
}
