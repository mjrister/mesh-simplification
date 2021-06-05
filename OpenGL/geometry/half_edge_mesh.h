#pragma once

#include <glm/mat4x4.hpp>

#include <map>
#include <memory>
#include <unordered_map>

namespace gfx {
	class Mesh;
}

namespace geometry {
	class Face;
	class HalfEdge;
	class Vertex;

	class HalfEdgeMesh {

	public:
		explicit HalfEdgeMesh(const gfx::Mesh& mesh);

		[[nodiscard]] const auto& Vertices() const { return vertices_; }
		[[nodiscard]] const auto& Edges() const { return edges_; }
		[[nodiscard]] const auto& Faces() const { return faces_; }
		[[nodiscard]] std::size_t NextVertexId() { return next_vertex_id_++; }

		operator gfx::Mesh() const;

		void CollapseEdge(const std::shared_ptr<HalfEdge>& edge01, const std::shared_ptr<Vertex>& v_new);

	private:
		std::map<std::size_t, std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
		glm::mat4 model_;
		std::size_t next_vertex_id_;
	};
}
