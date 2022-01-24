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

/**
 * \brief An edge centric data structure used to represent a triangle mesh.
 * \details A half-edge mesh is comprised of directional half-edges that refer to the next edge in a triangle in
 *          counter-clockwise order in addition to the vertex at the head of the edge. A half-edge also provides a
 *          pointer to its flip edge which represents the same edge in the opposite direction. Using just these
 *          three pointers, one can effectively traverse and modify edges in a triangle mesh.
 */
class HalfEdgeMesh {

public:
	/**
	 * \brief Initializes a half-edge mesh.
	 * \param mesh An indexed triangle mesh to construct the half-edge mesh from.
	 */
	explicit HalfEdgeMesh(const gfx::Mesh& mesh);

	/** \brief Defines the conversion operator back to a triangle mesh. */
	explicit operator gfx::Mesh() const;

	/** \brief Gets a mapping of mesh vertices by ID. */
	[[nodiscard]] const std::map<std::size_t, std::shared_ptr<Vertex>>& vertices() const noexcept { return vertices_; }

	/** \brief Gets a mapping of mesh half-edges by ID. */
	[[nodiscard]] const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges() const noexcept { return edges_; }

	/** \brief Gets a mapping of mesh faces by ID. */
	[[nodiscard]] const std::unordered_map<std::size_t, std::shared_ptr<Face>>& faces() const noexcept { return faces_; }

	/** \brief Gets a unique vertex ID that can be used to construct a new vertex in the half-edge mesh. */
	[[nodiscard]] std::size_t next_vertex_id() noexcept { return next_vertex_id_++; }

	/**
	 * \brief Collapses an edge into a single vertex and updates all incident edges to connect to that vertex.
	 * \param edge01 The edge from vertex \c v0 to \c v1 to collapse.
	 * \param v_new The vertex to collapse the edge onto.
	 */
	void CollapseEdge(const std::shared_ptr<HalfEdge>& edge01, const std::shared_ptr<Vertex>& v_new);

private:
	std::map<std::size_t, std::shared_ptr<Vertex>> vertices_;
	std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
	std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
	glm::mat4 model_transform_;
	std::size_t next_vertex_id_;
};
}
