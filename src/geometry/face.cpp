#include "geometry/face.h"

#include <algorithm>
#include <tuple>

#include <glm/geometric.hpp>

namespace gfx {

namespace {

/**
 * @brief Gets a canonical ordering of face vertices such that the vertex with the lowest ID.
 * @param v0,v1,v2 The face vertices.
 * @return A tuple consisting of @p v0, @p v1, @p v2 ordered by the lowest ID.
 * @note Preserves winding order. The is necessary to disambiguate equivalent face elements queries.
 */
auto GetMinVertexOrder(const std::shared_ptr<const Vertex>& v0,
                       const std::shared_ptr<const Vertex>& v1,
                       const std::shared_ptr<const Vertex>& v2) {
  const auto min_id = std::min({v0->id(), v1->id(), v2->id()});
  if (min_id == v0->id()) return make_tuple(v0, v1, v2);
  if (min_id == v1->id()) return make_tuple(v1, v2, v0);
  return make_tuple(v2, v0, v1);
}

}  // namespace

Face::Face(const std::shared_ptr<const Vertex>& v0,
           const std::shared_ptr<const Vertex>& v1,
           const std::shared_ptr<const Vertex>& v2) {
  tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);

  const auto edge01 = v1_.lock()->position() - v0_.lock()->position();
  const auto edge02 = v2_.lock()->position() - v0_.lock()->position();
  const auto normal = glm::cross(edge01, edge02);

  const auto normal_magnitude = glm::length(normal);
  assert(normal_magnitude != 0.0f);  // ensure face vertices are not collinear

  normal_ = normal / normal_magnitude;
  area_ = 0.5f * normal_magnitude;  // NOLINT(*-magic-numbers)
}

}  // namespace gfx
