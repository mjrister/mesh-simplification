#include "geometry/half_edge_mesh.h"

#include <cassert>
#include <ranges>

#include <glm/vec3.hpp>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace glm;
using namespace qem;
using namespace std;

namespace {

/**
 * \brief Creates a new half-edge and its associated flip edge.
 * \param v0,v1 The half-edge vertices.
 * \param edges A mapping of mesh half-edges by ID.
 * \return The half-edge connecting vertex \p v0 to \p v1.
 */
shared_ptr<HalfEdge> CreateHalfEdge(
	const shared_ptr<Vertex>& v0, const shared_ptr<Vertex>& v1, unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges) {

	const auto edge01_key = hash_value(*v0, *v1);
	const auto edge10_key = hash_value(*v1, *v0);

	// prevent the creation of duplicate edges
	if (const auto iterator = edges.find(edge01_key); iterator != edges.end()) {
		return iterator->second;
	}

	auto edge01 = make_shared<HalfEdge>(v1);
	const auto edge10 = make_shared<HalfEdge>(v0);

	edge01->SetFlip(edge10);
	edge10->SetFlip(edge01);

	edges.emplace(edge01_key, edge01);
	edges.emplace(edge10_key, edge10);

	return edge01;
}

/**
 * \brief Creates a new triangle in the half-edge mesh.
 * \param v0,v1,v2 The triangle vertices in counter-clockwise order.
 * \param edges A mapping of mesh half-edges by ID.
 * \return A triangle face representing vertices \p v0, \p v1, \p v2 in the half-edge mesh.
 */
shared_ptr<Face> CreateTriangle(
	const shared_ptr<Vertex>& v0,
	const shared_ptr<Vertex>& v1,
	const shared_ptr<Vertex>& v2,
	unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges) {

	const auto edge01 = CreateHalfEdge(v0, v1, edges);
	const auto edge12 = CreateHalfEdge(v1, v2, edges);
	const auto edge20 = CreateHalfEdge(v2, v0, edges);

	v0->SetEdge(edge20);
	v1->SetEdge(edge01);
	v2->SetEdge(edge12);

	edge01->SetNext(edge12);
	edge12->SetNext(edge20);
	edge20->SetNext(edge01);

	auto face012 = make_shared<Face>(v0, v1, v2);
	edge01->SetFace(face012);
	edge12->SetFace(face012);
	edge20->SetFace(face012);

	return face012;
}

/**
 * \brief Gets a half-edge connecting two vertices.
 * \param v0,v1 The half-edge vertices.
 * \param edges A mapping of mesh half-edges by ID.
 * \return The half-edge connecting \p v0 to \p v1.
 */
shared_ptr<HalfEdge> GetHalfEdge(
	const Vertex& v0, const Vertex& v1, const unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges) {
	const auto iterator = edges.find(hash_value(v0, v1));
	assert(iterator != edges.end());
	return iterator->second;
}

/**
 * \brief Deletes a vertex in the half-edge mesh.
 * \param vertex The vertex to delete.
 * \param vertices A mapping of mesh vertices by ID.
 */
void DeleteVertex(const Vertex& vertex, map<uint64_t, shared_ptr<Vertex>>& vertices) {
	const auto iterator = vertices.find(vertex.Id());
	assert(iterator != vertices.end());
	vertices.erase(iterator);
}

/**
 * \brief Deletes an edge in the half-edge mesh.
 * \param edge The half-edge to delete.
 * \param edges A mapping of mesh half-edges by ID.
 */
void DeleteEdge(const HalfEdge& edge, unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges) {
	for (const auto edge_key : {hash_value(edge), hash_value(*edge.Flip())}) {
		const auto iterator = edges.find(edge_key);
		assert(iterator != edges.end());
		edges.erase(iterator);
	}
}

/**
 * \brief Deletes a face in the half-edge mesh.
 * \param face The face to delete.
 * \param faces A mapping of mesh faces by ID.
 */
void DeleteFace(const Face& face, unordered_map<uint64_t, shared_ptr<Face>>& faces) {
	const auto iterator = faces.find(hash_value(face));
	assert(iterator != faces.end());
	faces.erase(iterator);
}

/**
 * \brief Attaches edges incident to a vertex to a new vertex.
 * \param v_target The vertex whose incident edges should be updated.
 * \param v_start The vertex opposite of \p v_target representing the first half-edge to process.
 * \param v_end The vertex opposite of \p v_target representing the last half-edge to process.
 * \param v_new The new vertex to attach edges to.
 * \param edges A mapping of mesh half-edges by ID.
 * \param faces A mapping of mesh faces by ID.
 */
void UpdateIncidentEdges(
	const Vertex& v_target,
	const Vertex& v_start,
	const Vertex& v_end,
	const shared_ptr<Vertex>& v_new,
	unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges,
	unordered_map<uint64_t, shared_ptr<Face>>& faces) {

	const auto edge_start = GetHalfEdge(v_target, v_start, edges);
	const auto edge_end = GetHalfEdge(v_target, v_end, edges);

	for (auto edge0i = edge_start; edge0i != edge_end;) {

		const auto edgeij = edge0i->Next();
		const auto edgej0 = edgeij->Next();

		const auto vi = edge0i->Vertex();
		const auto vj = edgeij->Vertex();

		const auto face_new = CreateTriangle(v_new, vi, vj, edges);
		faces.emplace(hash_value(*face_new), face_new);

		DeleteFace(*edge0i->Face(), faces);
		DeleteEdge(*edge0i, edges);

		edge0i = edgej0->Flip();
	}

	DeleteEdge(*edge_end, edges);
}

/**
 * \brief Computes a vertex normal by averaging its face normals weighted by surface area.
 * \param v0 The vertex to compute the normal for.
 * \return The weighted vertex normal.
 */
vec3 ComputeWeightedVertexNormal(const Vertex& v0) {
	vec3 normal{0.f};
	auto edgei0 = v0.Edge();
	do {
		const auto& face = edgei0->Face();
		normal += face->Normal() * face->Area();
		edgei0 = edgei0->Next()->Flip();
	} while (edgei0 != v0.Edge());
	return normalize(normal);
}
}

HalfEdgeMesh::HalfEdgeMesh(const Mesh& mesh) : model_transform_{mesh.ModelTransform()} {
	const auto& positions = mesh.Positions();
	const auto& indices = mesh.Indices();

	for (size_t i = 0; i < positions.size(); ++i) {
		vertices_.emplace(i, make_shared<Vertex>(i, positions[i]));
	}

	for (size_t i = 0; i < indices.size(); i += 3) {
		const auto& v0 = vertices_[indices[i]];
		const auto& v1 = vertices_[indices[i + 1]];
		const auto& v2 = vertices_[indices[i + 2]];
		const auto face012 = CreateTriangle(v0, v1, v2, edges_);
		faces_.emplace(hash_value(*face012), face012);
	}
}

HalfEdgeMesh::operator Mesh() const {

	vector<vec3> positions;
	positions.reserve(vertices_.size());

	vector<vec3> normals;
	normals.reserve(vertices_.size());

	vector<unsigned> indices;
	indices.reserve(faces_.size() * 3);

	unordered_map<uint64_t, unsigned> index_map;
	index_map.reserve(vertices_.size());

	for (auto i = 0u; const auto& vertex : vertices_ | views::values) {
		positions.push_back(vertex->Position());
		normals.push_back(ComputeWeightedVertexNormal(*vertex));
		index_map.emplace(vertex->Id(), i++); // map original vertex IDs to their new index positions
	}

	for (const auto& face : faces_ | views::values) {
		indices.push_back(index_map.at(face->V0()->Id()));
		indices.push_back(index_map.at(face->V1()->Id()));
		indices.push_back(index_map.at(face->V2()->Id()));
	}

	return Mesh{positions, {}, normals, indices, model_transform_};
}

void HalfEdgeMesh::Contract(const HalfEdge& edge01, const shared_ptr<Vertex>& v_new) {
	const auto edge10 = edge01.Flip();
	const auto v0 = edge10->Vertex();
	const auto v1 = edge01.Vertex();
	const auto v0_next = edge10->Next()->Vertex();
	const auto v1_next = edge01.Next()->Vertex();

	UpdateIncidentEdges(*v0, *v1_next, *v0_next, v_new, edges_, faces_);
	UpdateIncidentEdges(*v1, *v0_next, *v1_next, v_new, edges_, faces_);

	DeleteFace(*edge01.Face(), faces_);
	DeleteFace(*edge10->Face(), faces_);

	DeleteEdge(edge01, edges_);

	DeleteVertex(*v0, vertices_);
	DeleteVertex(*v1, vertices_);

	vertices_.emplace(v_new->Id(), v_new);
}
