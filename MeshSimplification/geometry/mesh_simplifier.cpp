#include "geometry/mesh_simplifier.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/norm.hpp>

#include "geometry/half_edge.h"
#include "geometry/half_edge_mesh.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

using namespace geometry;
using namespace glm;
using namespace std;

namespace {

	/**
	 * \brief Gets a canonical representation of a half-edge used to disambiguate between its flip edge.
	 * \param edge The half-edge to disambiguate.
	 * \return For two vertices connected by an edge, returns the half-edge pointing to the vertex with the smallest ID.
	 */
	shared_ptr<HalfEdge> GetMinEdge(const shared_ptr<HalfEdge>& edge) {
		return min<>(edge, edge->Flip(), [](const auto& edge01, const auto& edge10) {
			return edge01->Vertex()->Id() < edge10->Vertex()->Id();
		});
	}

	/**
	 * \brief Computes a vertex normal by averaging its face normals.
	 * \param v0 The vertex to evaluate.
	 * \return The vertex normal.
	 */
	vec3 AverageFaceNormals(const shared_ptr<Vertex>& v0) {
		vec3 normal{0.f};
		uint8_t face_count = 0;
		auto edgei0 = v0->Edge();
		do {
			normal += edgei0->Face()->Normal();
			edgei0 = edgei0->Next()->Flip();
			++face_count;
		} while (edgei0 != v0->Edge());
		return normalize(normal / static_cast<float>(face_count));
	}

	/**
	 * \brief Computes the error quadric for a vertex.
	 * \param vertex The vertex to evaluate.
	 * \return The summation of quadrics for all triangles incident to \p vertex.
	 */
	mat4 ComputeQuadric(const Vertex& vertex) {
		mat4 quadric{0.f};
		auto edgei0 = vertex.Edge();
		do {
			const auto& position = vertex.Position();
			const auto& normal = edgei0->Face()->Normal();
			const vec4 plane{normal, -dot(position, normal)};
			quadric += outerProduct(plane, plane);
			edgei0 = edgei0->Next()->Flip();
		} while (edgei0 != vertex.Edge());
		return quadric;
	}

	/**
	 * \brief Determines the optimal vertex position for an edge contraction.
	 * \param vertex_id The ID to use for the newly created vertex.
	 * \param edge01 The half-edge to evaluate.
	 * \param quadrics A mapping of error quadrics by vertex ID.
	 * \return The optimal vertex and cost associated with collapsing \p edge01.
	 */
	pair<shared_ptr<Vertex>, float> GetOptimalEdgeContractionVertex(
		const size_t vertex_id, const HalfEdge& edge01, const unordered_map<size_t, mat4>& quadrics) {

		const auto v0 = edge01.Flip()->Vertex();
		const auto v1 = edge01.Vertex();

		const auto& q0 = quadrics.at(v0->Id());
		const auto& q1 = quadrics.at(v1->Id());
		const auto q01 = q0 + q1;

		const mat3 Q{q01};
		const vec3 b = column(q01, 3);
		const auto d = q01[3][3];

		// if the upper 3x3 matrix of the error quadric is not invertible, average the edge vertices
		static constexpr auto epsilon = numeric_limits<float>::epsilon();
		if (abs(determinant(Q)) < epsilon || abs(d) < epsilon) {
			const auto position = (v0->Position() + v1->Position()) / 2.f;
			return {make_shared<Vertex>(vertex_id, position), 0.f};
		}

		const auto Q_inv = inverse(Q);
		const auto D_inv = column(mat4{Q_inv}, 3, vec4{-1.f / d * Q_inv * b, 1.f / d});

		auto position = D_inv * vec4{0.f, 0.f, 0.f, 1.f};
		position /= position.w;
		const auto cost = dot(position, q01 * position);

		return {make_shared<Vertex>(vertex_id, position), cost};
	}

	/**
	 * \brief Determines if the removal of an edge will cause the mesh to degenerate.
	 * \param edge01 The half-edge to evaluate.
	 * \return \c true if the removal of \p edge01 will produce a non-manifold, otherwise \c false.
	 */
	bool WillDegenerate(const shared_ptr<HalfEdge>& edge01) {
		const auto v0 = edge01->Flip()->Vertex();
		const auto v1_next = edge01->Next()->Vertex();
		const auto v0_next = edge01->Flip()->Next()->Vertex();
		unordered_map<size_t, shared_ptr<Vertex>> neighborhood;

		for (auto iterator = edge01->Next(); iterator != edge01->Flip(); iterator = iterator->Flip()->Next()) {
			if (const auto vertex = iterator->Vertex(); vertex != v0 && vertex != v1_next && vertex != v0_next) {
				neighborhood.emplace(hash_value(*vertex), vertex);
			}
		}

		for (auto iterator = edge01->Flip()->Next(); iterator != edge01; iterator = iterator->Flip()->Next()) {
			if (const auto vertex = iterator->Vertex(); neighborhood.count(hash_value(*vertex))) {
				return true;
			}
		}

		return false;
	}

	/** \brief Represents an edge contraction priority queue entry. */
	struct EdgeContraction {

		EdgeContraction(
			HalfEdgeMesh& mesh,
			const shared_ptr<HalfEdge>& edge,
			const unordered_map<size_t, mat4>& quadrics) : edge{edge} {
			tie(vertex, cost) = GetOptimalEdgeContractionVertex(mesh.NextVertexId(), *edge, quadrics);
		}

		/** \brief The edge to be collapsed. */
		const shared_ptr<HalfEdge> edge;

		/** \brief The optimal vertex position that minimizes the cost of collapsing this edge. */
		shared_ptr<Vertex> vertex;

		/** \brief The associated cost of collapsing this edge. */
		float cost = numeric_limits<float>::infinity();

		/**
		 * \brief This is used as a workaround for priority_queue not providing a method to update an existing
		 *        entry's priority. As edges are updated in the mesh, duplicated entries may be inserted in the queue
		 *        and this property will be used to determine if an entry refers to the most recent edge update.
		 */
		bool valid = true;
	};
}

gfx::Mesh mesh::Simplify(const gfx::Mesh& mesh, const float rate) {

	if (rate < 0.f || rate > 1.f) throw invalid_argument{format("Invalid mesh simplification rate {}", rate)};

	const auto start_time = chrono::high_resolution_clock::now();
	HalfEdgeMesh half_edge_mesh{mesh};

	// compute error quadrics for each vertex
	unordered_map<size_t, mat4> quadrics;
	for (const auto& [vertex_id, vertex] : half_edge_mesh.Vertices()) {
		quadrics.emplace(vertex_id, ComputeQuadric(*vertex));
	}

	// use a priority queue to sort edge contraction candidates by the associate cost of collapsing that edge
	constexpr auto min_heap_comparator = [](
		const shared_ptr<EdgeContraction>& lhs,
		const shared_ptr<EdgeContraction>& rhs) { return lhs->cost > rhs->cost; };
	priority_queue<
		shared_ptr<EdgeContraction>,
		vector<shared_ptr<EdgeContraction>>,
		decltype(min_heap_comparator)> edge_contractions{min_heap_comparator};

	// this is used to invalidate existing priority queue entries as edges are updated or removed from the mesh
	unordered_map<size_t, shared_ptr<EdgeContraction>> valid_edges;

	// compute the optimal vertex position that minimizes the cost of collapsing each edge
	for (const auto& edge : half_edge_mesh.Edges() | views::values) {
		const auto min_edge = GetMinEdge(edge);
		if (const auto min_edge_key = hash_value(*min_edge); !valid_edges.contains(min_edge_key)) {
			const auto edge_contraction = make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
			edge_contractions.push(edge_contraction);
			valid_edges.emplace(min_edge_key, edge_contraction);
		}
	}

	// stop mesh simplification if the number of triangles has been sufficiently reduced
	const auto initial_face_count = static_cast<float>(half_edge_mesh.Faces().size());
	const auto target_face_count = initial_face_count * (1.f - rate);
	const auto should_stop = [&]() {
		const auto face_count = static_cast<float>(half_edge_mesh.Faces().size());
		return face_count < target_face_count;
	};

	while (!edge_contractions.empty() && !should_stop()) {
		const auto& edge_contraction = edge_contractions.top();
		const auto& edge01 = edge_contraction->edge;
		const auto& v_new = edge_contraction->vertex;

		if (edge_contraction->valid && !WillDegenerate(edge01)) {
			const auto v0 = edge01->Flip()->Vertex();
			const auto v1 = edge01->Vertex();

			// remove the edge from the mesh and attach incident edges to the new vertex
			half_edge_mesh.CollapseEdge(edge01, v_new);
			v_new->SetNormal(AverageFaceNormals(v_new));

			// compute the error quadric for the new vertex
			const auto& q0 = quadrics.at(v0->Id());
			const auto& q1 = quadrics.at(v1->Id());
			quadrics.emplace(v_new->Id(), q0 + q1);

			// invalidate entries in the priority queue that were removed during the edge contraction
			for (const auto& vertex : {v0, v1}) {
				auto edge = vertex->Edge();
				do {
					const auto min_edge = GetMinEdge(edge);
					if (const auto iterator = valid_edges.find(hash_value(*min_edge)); iterator != valid_edges.end()) {
						iterator->second->valid = false;
						valid_edges.erase(iterator);
					}
					edge = edge->Next()->Flip();
				} while (edge != vertex->Edge());
			}

			// add new edge contraction candidates for edges affected by the edge contraction
			unordered_map<size_t, shared_ptr<HalfEdge>> visited_edges;
			const auto& vi = v_new;
			auto edgeji = vi->Edge();
			do {
				const auto vj = edgeji->Flip()->Vertex();
				auto edgekj = vj->Edge();
				do {
					const auto min_edge = GetMinEdge(edgekj);
					if (const auto min_edge_key = hash_value(*min_edge); !visited_edges.contains(min_edge_key)) {
						if (auto iterator = valid_edges.find(min_edge_key); iterator != valid_edges.end()) {
							// invalidate existing edge contraction candidate in the priority queue
							iterator->second->valid = false;
						}
						const auto new_edge_contraction =
							make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
						valid_edges[min_edge_key] = new_edge_contraction;
						edge_contractions.emplace(new_edge_contraction);
						visited_edges.emplace(min_edge_key, min_edge);
					}
					edgekj = edgekj->Next()->Flip();
				} while (edgekj != vj->Edge());
				edgeji = edgeji->Next()->Flip();
			} while (edgeji != vi->Edge());
		}

		edge_contractions.pop();
	}

	const auto end_time = chrono::high_resolution_clock::now();
	cout << std::format(
		"Mesh simplified from {} to {} triangles in {} seconds\n",
		initial_face_count,
		half_edge_mesh.Faces().size(),
		chrono::duration<float>{end_time - start_time}.count());

	return half_edge_mesh;
}
