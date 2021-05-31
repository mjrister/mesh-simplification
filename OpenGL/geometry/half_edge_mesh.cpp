#include "half_edge_mesh.h"

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> CreateHalfEdge(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges_by_id) {

		const auto edge01_id = HalfEdge::GetHalfEdgeId(*v0, *v1);
		if (const auto iterator = edges_by_id.find(edge01_id); iterator != edges_by_id.end()) {
			return iterator->second;
		}

		auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);

		edges_by_id.emplace(edge01->Id(), edge01);
		edges_by_id.emplace(edge10->Id(), edge10);

		return edge01;
	}

	std::shared_ptr<Face> CreateTriangle(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		const std::shared_ptr<Vertex>& v2,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges_by_id) {

		const auto edge01 = CreateHalfEdge(v0, v1, edges_by_id);
		const auto edge12 = CreateHalfEdge(v1, v2, edges_by_id);
		const auto edge20 = CreateHalfEdge(v2, v0, edges_by_id);

		v0->SetEdge(edge20);
		v1->SetEdge(edge01);
		v2->SetEdge(edge12);

		edge01->SetNext(edge12);
		edge12->SetNext(edge20);
		edge20->SetNext(edge01);

		auto face012 = std::make_shared<Face>(v0, v1, v2);
		face012->SetEdge(edge01);

		edge01->SetFace(face012);
		edge12->SetFace(face012);
		edge20->SetFace(face012);

		return face012;
	}
}

HalfEdgeMesh::HalfEdgeMesh(const gfx::Mesh& mesh) {
	const auto& positions = mesh.Positions();
	const auto& normals = mesh.Normals();
	const auto& indices = mesh.Indices();

	for (std::size_t i = 0; i < positions.size(); ++i) {
		vertices_by_id_.emplace(i, std::make_shared<Vertex>(i, positions[i], normals[i]));
	}

	for (std::size_t i = 0; i < indices.size(); i += 3) {
		const auto v0 = vertices_by_id_[indices[i]];
		const auto v1 = vertices_by_id_[indices[i + 1]];
		const auto v2 = vertices_by_id_[indices[i + 2]];
		const auto face012 = CreateTriangle(v0, v1, v2, edges_by_id_);
		faces_by_id_.emplace(face012->Id(), face012);
	}
}

gfx::Mesh HalfEdgeMesh::ToMesh() {

	std::vector<glm::vec4> positions;
	positions.reserve(vertices_by_id_.size());

	std::vector<glm::vec3> normals;
	normals.reserve(vertices_by_id_.size());

	for (const auto& [_, vertex] : vertices_by_id_) {
		positions.push_back(vertex->Position());
		normals.push_back(vertex->Normal());
	}

	std::vector<GLuint> indices;
	indices.reserve(faces_by_id_.size() * 3);

	for (const auto& [_, face] : faces_by_id_) {
		indices.push_back(static_cast<GLuint>(face->V0()->Id()));
		indices.push_back(static_cast<GLuint>(face->V1()->Id()));
		indices.push_back(static_cast<GLuint>(face->V2()->Id()));
	}

	return gfx::Mesh{positions, {}, normals, indices};
}
