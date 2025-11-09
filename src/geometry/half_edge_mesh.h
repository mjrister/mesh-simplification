#ifndef GEOMETRY_HALF_EDGE_MESH_H_
#define GEOMETRY_HALF_EDGE_MESH_H_

#include <map>
#include <memory>
#include <unordered_map>

#include <glm/mat4x4.hpp>

#include "geometry/half_edge.h"

namespace gfx {
class Face;
class HalfEdge;
class Mesh;
class Vertex;

/**
 * @brief An edge centric data structure used to represent a triangle mesh.
 * @details A half-edge mesh is comprised of directional half-edges that refer to the next edge in a triangle in
 *          counter-clockwise order in addition to the vertex at the head of the edge. A half-edge also provides a
 *          pointer to its flip edge which represents the same edge in the opposite direction. Using just these
 *          three pointers, one can effectively traverse and modify edges in a triangle mesh.
 */
class HalfEdgeMesh {
public:
  /**
   * @brief Creates a half-edge mesh.
   * @param mesh An indexed triangle mesh to construct the half-edge mesh from.
   */
  explicit HalfEdgeMesh(const Mesh& mesh);

  /** @brief Defines the conversion operator back to a triangle mesh. */
  explicit operator Mesh() const;

  /** @brief Gets a mapping of mesh vertices by ID. */
  [[nodiscard]] const std::map<int, SharedVertex>& vertices() const noexcept { return vertices_; }

  /** @brief Gets a mapping of mesh half-edges by hash key. */
  [[nodiscard]] const std::unordered_map<std::size_t, SharedHalfEdge>& edges() const noexcept { return edges_; }

  /** @brief Gets a mapping of mesh faces by hash key. */
  [[nodiscard]] const std::unordered_map<std::size_t, SharedFace>& faces() const noexcept { return faces_; }

  /**
   * @brief Performs edge contraction.
   * @details Edge contraction consists of removing an edge from the mesh by merging its two vertices into a
   *          single vertex and updating edges incident to each endpoint to connect to that new vertex.
   * @param edge01 The edge from vertex @c v0 to @c v1 to remove.
   * @param v_new The new vertex to update incident edges to.
   */
  void Contract(const HalfEdge& edge01, const SharedVertex& v_new);

private:
  std::map<int, SharedVertex> vertices_;
  std::unordered_map<std::size_t, SharedHalfEdge> edges_;
  std::unordered_map<std::size_t, SharedFace> faces_;
  glm::mat4 model_transform_;
};

}  // namespace gfx

#endif  // GEOMETRY_HALF_EDGE_MESH_H_
