#include "geometry/half_edge_mesh.h"

#include <sstream>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/hash.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace geometry;

namespace {
	std::shared_ptr<HalfEdge> CreateHalfEdge(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		const auto edge01_key = hash_value(*v0, *v1);
		const auto edge10_key = hash_value(*v1, *v0);

		if (const auto iterator = edges.find(edge01_key); iterator != edges.end()) {
			return iterator->second;
		}

		auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);

		edges.emplace(edge01_key, edge01);
		edges.emplace(edge10_key, edge10);

		return edge01;
	}

	std::shared_ptr<Face> CreateTriangle(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		const std::shared_ptr<Vertex>& v2,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		const auto edge01 = CreateHalfEdge(v0, v1, edges);
		const auto edge12 = CreateHalfEdge(v1, v2, edges);
		const auto edge20 = CreateHalfEdge(v2, v0, edges);

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

	std::shared_ptr<HalfEdge> GetHalfEdge(
		const Vertex& v0, const Vertex& v1, const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		if (const auto iterator = edges.find(hash_value(v0, v1)); iterator == edges.end()) {
			std::ostringstream oss;
			oss << "Attempted to retrieve a nonexistent edge (" << v0.Id() << ',' << v1.Id() << ')';
			throw std::invalid_argument("Attempted to retrieve a nonexistent edge");
		} else {
			return iterator->second;
		}
	}

	void DeleteVertex(const Vertex& v0, std::map<std::size_t, std::shared_ptr<Vertex>>& vertices) {

		if (const auto iterator = vertices.find(v0.Id()); iterator == vertices.end()) {
			std::ostringstream oss;
			oss << "Attempted to delete a nonexistent vertex " << v0;
			throw std::invalid_argument{oss.str()};
		} else {
			vertices.erase(iterator);
		}
	}

	void DeleteEdge(const HalfEdge& edge, std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		for (const auto& edge_key : {hash_value(edge), hash_value(*edge.Flip())}) {
			if (const auto iterator = edges.find(edge_key); iterator == edges.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent edge " << edge;
				throw std::invalid_argument{oss.str()};
			} else {
				edges.erase(iterator);
			}
		}
	}

	void DeleteFace(const Face& face, std::unordered_map<std::size_t, std::shared_ptr<Face>>& faces) {

		if (const auto iterator = faces.find(hash_value(face)); iterator == faces.end()) {
			std::ostringstream oss;
			oss << "Attempted to delete a nonexistent face " << face;
			throw std::invalid_argument{oss.str()};
		} else {
			faces.erase(iterator);
		}
	}

	void CollapseIncidentTriangles(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v_start,
		const std::shared_ptr<Vertex>& v_end,
		const std::shared_ptr<Vertex>& v_new,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges,
		std::unordered_map<std::size_t, std::shared_ptr<Face>>& faces) {

		const auto edge_start = GetHalfEdge(*v0, *v_start, edges);
		const auto edge_end = GetHalfEdge(*v0, *v_end, edges);

		for (auto edge0i = edge_start; edge0i != edge_end;) {

			const auto edgeij = edge0i->Next();
			const auto edgej0 = edgeij->Next();

			const auto vi = edge0i->Vertex();
			const auto vj = edgeij->Vertex();

			const auto face_new = CreateTriangle(v_new, vi, vj, edges);
			faces.emplace(hash_value(*face_new), face_new);

			DeleteEdge(*edge0i, edges);
			DeleteFace(*edge0i->Face(), faces);

			edge0i = edgej0->Flip();
		}

		DeleteEdge(*edge_end, edges);
	}
}

HalfEdgeMesh::HalfEdgeMesh(const gfx::Mesh& mesh) {
	const auto& positions = mesh.Positions();
	const auto& normals = mesh.Normals();
	const auto& indices = mesh.Indices();

	for (std::size_t i = 0; i < positions.size(); ++i) {
		vertices_.emplace(i, std::make_shared<Vertex>(i, positions[i], normals[i]));
	}

	for (std::size_t i = 0; i < indices.size(); i += 3) {

		const auto v0 = vertices_[indices[i]];
		const auto v1 = vertices_[indices[i + 1]];
		const auto v2 = vertices_[indices[i + 2]];

		const auto face012_key = hash_value(*v0, *v1, *v2);
		const auto face012 = CreateTriangle(v0, v1, v2, edges_);
		faces_.emplace(face012_key, face012);
	}

	next_vertex_id_ = positions.size();
}

HalfEdgeMesh::operator gfx::Mesh() const {

	std::vector<glm::vec3> positions;
	positions.reserve(vertices_.size());

	std::vector<glm::vec3> normals;
	normals.reserve(vertices_.size());

	std::vector<GLuint> indices;
	indices.reserve(faces_.size() * 3);

	for (const auto& [_, vertex] : vertices_) {
		positions.push_back(vertex->Position());
		normals.push_back(vertex->Normal());
	}

	for (const auto& [_, face] : faces_) {
		indices.push_back(static_cast<GLuint>(face->V0()->Id()));
		indices.push_back(static_cast<GLuint>(face->V1()->Id()));
		indices.push_back(static_cast<GLuint>(face->V2()->Id()));
	}

	return gfx::Mesh{positions, {}, normals, indices};
}

void HalfEdgeMesh::CollapseEdge(
	const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v_new) {

	const auto edge01 = GetHalfEdge(*v0, *v1, edges_);
	const auto edge10 = edge01->Flip();

	const auto vt = edge01->Next()->Vertex();
	const auto vb = edge10->Next()->Vertex();

	const auto face01t = edge01->Face();
	const auto face10b = edge10->Face();

	CollapseIncidentTriangles(v0, vt, vb, v_new, edges_, faces_);
	CollapseIncidentTriangles(v1, vb, vt, v_new, edges_, faces_);

	DeleteEdge(*edge01, edges_);

	DeleteFace(*face01t, faces_);
	DeleteFace(*face10b, faces_);

	DeleteVertex(*v0, vertices_);
	DeleteVertex(*v1, vertices_);

	vertices_.emplace(v_new->Id(), v_new);
}
