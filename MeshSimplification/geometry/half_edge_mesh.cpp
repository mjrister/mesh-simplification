#include "geometry/half_edge_mesh.h"

#include <format>
#include <ranges>

#include <glm/vec3.hpp>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace geometry;
using namespace gfx;
using namespace glm;
using namespace std;

namespace {

/**
 * \brief Creates a new half-edge and its associated flip edge.
 * \param v0,v1 The half-edge vertices.
 * \param edges A mapping of mesh half-edges by ID.
 * \return The half-edge connecting vertex \p v0 to \p v1.
 */
shared_ptr<HalfEdge> CreateHalfEdge(
	const shared_ptr<Vertex>& v0, const shared_ptr<Vertex>& v1, unordered_map<size_t, shared_ptr<HalfEdge>>& edges) {

	const auto edge01_key = hash_value(*v0, *v1);
	const auto edge10_key = hash_value(*v1, *v0);

	// prevent the creation of duplicate edges
	if (const auto iterator = edges.find(edge01_key); iterator != edges.end()) {
		return iterator->second;
	}

	auto edge01 = make_shared<HalfEdge>(v1);
	const auto edge10 = make_shared<HalfEdge>(v0);

	edge01->set_flip(edge10);
	edge10->set_flip(edge01);

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
	unordered_map<size_t, shared_ptr<HalfEdge>>& edges) {

	const auto edge01 = CreateHalfEdge(v0, v1, edges);
	const auto edge12 = CreateHalfEdge(v1, v2, edges);
	const auto edge20 = CreateHalfEdge(v2, v0, edges);

	v0->set_edge(edge20);
	v1->set_edge(edge01);
	v2->set_edge(edge12);

	edge01->set_next(edge12);
	edge12->set_next(edge20);
	edge20->set_next(edge01);

	auto face012 = make_shared<Face>(v0, v1, v2);
	edge01->set_face(face012);
	edge12->set_face(face012);
	edge20->set_face(face012);

	return face012;
}

/**
 * \brief Gets a half-edge connecting two vertices.
 * \param v0,v1 The half-edge vertices.
 * \param edges A mapping of mesh half-edges by ID.
 * \return The half-edge connecting \p v0 to \p v1.
 * \throw invalid_argument Indicates no edge connecting \p v0 to \p v1 exists in \p edges.
 */
shared_ptr<HalfEdge> GetHalfEdge(
	const Vertex& v0, const Vertex& v1, const unordered_map<size_t, shared_ptr<HalfEdge>>& edges) {

	if (const auto iterator = edges.find(hash_value(v0, v1)); iterator == edges.end()) {
		throw invalid_argument(format("Attempted to retrieve a nonexistent edge: ({},{})", v0, v1));
	} else {
		return iterator->second;
	}
}

/**
 * \brief Deletes a vertex in the half-edge mesh.
 * \param vertex The vertex to delete.
 * \param vertices A mapping of mesh vertices by ID.
 * \throw invalid_argument Indicates \p vertex does not exist in \p vertices.
 */
void DeleteVertex(const Vertex& vertex, map<size_t, shared_ptr<Vertex>>& vertices) {

	if (const auto iterator = vertices.find(vertex.id()); iterator == vertices.end()) {
		throw invalid_argument{format("Attempted to delete a nonexistent vertex: {}", vertex)};
	} else {
		vertices.erase(iterator);
	}
}

/**
 * \brief Deletes an edge in the half-edge mesh.
 * \param edge The half-edge to delete.
 * \param edges A mapping of mesh half-edges by ID.
 * \throw invalid_argument Indicates \p edge does not exist in \p edges.
 */
void DeleteEdge(const HalfEdge& edge, unordered_map<size_t, shared_ptr<HalfEdge>>& edges) {

	for (const auto& edge_key : {hash_value(edge), hash_value(*edge.flip())}) {
		if (const auto iterator = edges.find(edge_key); iterator == edges.end()) {
			throw invalid_argument{format("Attempted to delete a nonexistent edge: {}", edge)};
		} else {
			edges.erase(iterator);
		}
	}
}

/**
 * \brief Deletes a face in the half-edge mesh.
 * \param face The face to delete.
 * \param faces A mapping of mesh faces by ID.
 * \throw invalid_argument Indicates \p face does not exist in \p faces.
 */
void DeleteFace(const Face& face, unordered_map<size_t, shared_ptr<Face>>& faces) {

	if (const auto iterator = faces.find(hash_value(face)); iterator == faces.end()) {
		throw invalid_argument{format("Attempted to delete a nonexistent face: {}", face)};
	} else {
		faces.erase(iterator);
	}
}

/**
 * \brief Attaches triangles incident to an edge's vertex to a new vertex.
 * \param v_target The vertex to be removed whose incident edges require updating.
 * \param v_start The vertex opposite of \p v_target representing the first half-edge to process.
 * \param v_end The vertex opposite of \p v_target representing the last half-edge to process.
 * \param v_new The new vertex to attach incident edges to.
 * \param edges A mapping of mesh half-edges by ID.
 * \param faces A mapping of mesh faces by ID.
 */
void UpdateIncidentTriangles(
	const shared_ptr<Vertex>& v_target,
	const shared_ptr<Vertex>& v_start,
	const shared_ptr<Vertex>& v_end,
	const shared_ptr<Vertex>& v_new,
	unordered_map<size_t, shared_ptr<HalfEdge>>& edges,
	unordered_map<size_t, shared_ptr<Face>>& faces) {

	const auto edge_start = GetHalfEdge(*v_target, *v_start, edges);
	const auto edge_end = GetHalfEdge(*v_target, *v_end, edges);

	for (auto edge0i = edge_start; edge0i != edge_end;) {

		const auto edgeij = edge0i->next();
		const auto edgej0 = edgeij->next();

		const auto vi = edge0i->vertex();
		const auto vj = edgeij->vertex();

		const auto face_new = CreateTriangle(v_new, vi, vj, edges);
		faces.emplace(hash_value(*face_new), face_new);

		DeleteFace(*edge0i->face(), faces);
		DeleteEdge(*edge0i, edges);

		edge0i = edgej0->flip();
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
	auto edgei0 = v0.edge();
	do {
		const auto& face = edgei0->face();
		normal += face->normal() * face->area();
		edgei0 = edgei0->next()->flip();
	} while (edgei0 != v0.edge());
	return normalize(normal);
}
}

HalfEdgeMesh::HalfEdgeMesh(const Mesh& mesh) : model_transform_{mesh.model_transform()} {
	const auto& positions = mesh.positions();
	const auto& indices = mesh.indices();

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

	next_vertex_id_ = positions.size();
}

HalfEdgeMesh::operator Mesh() const {

	vector<vec3> positions;
	positions.reserve(vertices_.size());

	vector<vec3> normals;
	normals.reserve(vertices_.size());

	vector<GLuint> indices;
	indices.reserve(faces_.size() * 3);

	unordered_map<size_t, GLuint> index_map;
	for (GLuint i = 0; const auto& vertex : vertices_ | views::values) {
		positions.push_back(vertex->position());
		normals.push_back(ComputeWeightedVertexNormal(*vertex));
		index_map.emplace(vertex->id(), i++);
	}

	for (const auto& face : faces_ | views::values) {
		indices.push_back(index_map.at(face->v0()->id()));
		indices.push_back(index_map.at(face->v1()->id()));
		indices.push_back(index_map.at(face->v2()->id()));
	}

	return Mesh{positions, {}, normals, indices, model_transform_};
}

void HalfEdgeMesh::CollapseEdge(const shared_ptr<HalfEdge>& edge01, const shared_ptr<Vertex>& v_new) {
	const auto edge10 = edge01->flip();
	const auto v0 = edge10->vertex();
	const auto v1 = edge01->vertex();
	const auto v0_next = edge10->next()->vertex();
	const auto v1_next = edge01->next()->vertex();

	UpdateIncidentTriangles(v0, v1_next, v0_next, v_new, edges_, faces_);
	UpdateIncidentTriangles(v1, v0_next, v1_next, v_new, edges_, faces_);

	DeleteFace(*edge01->face(), faces_);
	DeleteFace(*edge10->face(), faces_);

	DeleteEdge(*edge01, edges_);

	DeleteVertex(*v0, vertices_);
	DeleteVertex(*v1, vertices_);

	vertices_.emplace(v_new->id(), v_new);
}
