#include "geometry/half_edge_mesh.h"

#include <sstream>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace geometry;

namespace {
	std::size_t hash(const Vertex& v0) {
		std::size_t seed = 0x3E9EB221;
		seed ^= (seed << 6) + (seed >> 2) + 0x3573AC13 + std::hash<std::uint64_t>{}(v0.Id());
		return seed;
	}

	std::size_t hash(const Vertex& v0, const Vertex& v1) {
		std::size_t seed = 0x32C95994;
		seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + hash(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + hash(v1);
		return seed;
	}

	std::size_t hash(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		std::size_t seed = 0x230402B5;
		seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + hash(v0);
		seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + hash(v1);
		seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + hash(v2);
		return seed;
	}

	std::shared_ptr<HalfEdge> CreateHalfEdge(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		const auto edge01_key = hash(*v0, *v1);
		const auto edge10_key = hash(*v1, *v0);

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
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		if (const auto iterator = edges.find(hash(*v0, *v1)); iterator == edges.end()) {
			std::ostringstream oss;
			oss << "Attempted to delete a nonexistent edge (" << v0->Id() << ',' << v1->Id() << ')';
			throw std::invalid_argument("Attempted to retrieve a nonexistent edge");
		}
		else {
			return iterator->second;
		}
	}

	void DeleteVertex(const std::shared_ptr<Vertex>& v0, std::map<std::size_t, std::shared_ptr<Vertex>>& vertices) {
		if (const auto iterator = vertices.find(v0->Id()); iterator == vertices.end()) {
			std::ostringstream oss;
			oss << "Attempted to delete a nonexistent vertex " << v0->Id();
			throw std::invalid_argument{oss.str()};
		}
		else {
			vertices.erase(iterator);
		}
	}

	void DeleteEdge(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		for (const auto& edge_key : {hash(*v0, *v1), hash(*v1, *v0)}) {
			if (const auto iterator = edges.find(edge_key); iterator == edges.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent edge (" << v0->Id() << ',' << v1->Id() << ')';
				throw std::invalid_argument{ oss.str() };
			}
			else {
				edges.erase(iterator);
			}
		}
	}

	void DeleteFace(
		const std::shared_ptr<Vertex>& v0,
		const std::shared_ptr<Vertex>& v1,
		const std::shared_ptr<Vertex>& v2,
		std::unordered_map<std::size_t, std::shared_ptr<Face>>& faces) {

		const Face face012{v0, v1, v2};
		const auto face012_key = hash(*face012.V0(), *face012.V1(), *face012.V2());

		if (const auto iterator = faces.find(face012_key); iterator == faces.end()) {
			std::ostringstream oss;
			oss << "Attempted to delete a nonexistent face (" << v0->Id() << ',' << v1->Id() << ',' << v2->Id() << ')';
			throw std::invalid_argument{ oss.str() };
		}
		else {
			faces.erase(iterator);
		}
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

		const auto face012_key = hash(*v0, *v1, *v2);
		const auto face012 = CreateTriangle(v0, v1, v2, edges_);
		faces_.emplace(face012_key, face012);
	}
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

//void HalfEdgeMesh::CollapseEdge(
//	const Vertex& v0, const Vertex& v1, const Vertex& v_new) {
//
//	const auto edge01_key = hash(v0, v1);
//	const auto edge01_iterator = edges_.find(edge01_key);
//
//	if (edge01_iterator == edges_.end()) {
//		std::ostringstream oss;
//		oss << "Attempted to collapse a nonexistent edge (" << v0 << ',' << v1 << ')';
//		throw std::invalid_argument{oss.str()};
//	}
//
//	const auto edge01 = GetHalfEdge(v0, v1);
//	const auto edge10 = edge01->Flip();
//
//	const auto vt = edge01->Next()->Vertex();
//	const auto vb = edge10->Next()->Vertex();
//
//	const auto face01t = edge01->Face();
//	const auto face10b = edge10->Face();
//
//	CollapseIncidentTriangles(v0, vt, vb, v_new);
//	CollapseIncidentTriangles(v1, vb, vt, v_new);
//
//	DeleteEdge(edge01);
//
//	DeleteFace(face01t);
//	DeleteFace(face10b);
//
//	DeleteVertex(v0);
//	DeleteVertex(v1);
//}
