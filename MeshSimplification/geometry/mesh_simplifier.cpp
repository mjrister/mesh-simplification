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

#include "geometry/half_edge.h"
#include "geometry/half_edge_mesh.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

namespace {
	constexpr auto epsilon = std::numeric_limits<float>::epsilon();

	/**
	 * \brief Gets a canonical representation of a half-edge. Used to disambiguate between its flip edge.
	 * \param edge The half-edge to disambiguate.
	 * \return For two vertices connected by an edge, returns the half-edge pointing to the vertex with the smallest ID.
	 */
	std::shared_ptr<geometry::HalfEdge> GetMinEdge(const std::shared_ptr<geometry::HalfEdge>& edge) {
		return std::min<>(edge, edge->Flip(), [](const auto& edge01, const auto& edge10) {
			return edge01->Vertex()->Id() < edge10->Vertex()->Id();
		});
	}

	/**
	 * \brief Averages two vertices.
	 * \param vertex_id The ID of the vertex to be created.
	 * \param v0,v1 The vertices to average.
	 * \return A new vertex containing the averaged position and normal between \p v0 and \p v1.
	 */
	std::shared_ptr<geometry::Vertex> AverageVertices(
		const std::size_t vertex_id, const geometry::Vertex& v0, const geometry::Vertex& v1) {
		const auto position = (v0.Position() + v1.Position()) / 2.f;
		const auto normal = (v0.Normal() + v1.Normal()) / 2.f;
		return std::make_shared<geometry::Vertex>(vertex_id, position, normal);
	}

	/**
	 * \brief Computes the error quadric for a vertex.
	 * \param vertex The vertex to compute the error quadric for.
	 * \return The summation of quadrics for all triangles incident to \p vertex.
	 */
	glm::mat4 ComputeQuadric(const geometry::Vertex& vertex) {
		glm::mat4 quadric{0.f};
		auto iterator = vertex.Edge();
		do {
			const auto& position = vertex.Position();
			const auto& normal = iterator->Face()->Normal();
			const glm::vec4 plane{normal, -glm::dot(position, normal)};
			quadric += glm::outerProduct(plane, plane);
			iterator = iterator->Next()->Flip();
		} while (iterator != vertex.Edge());
		return quadric;
	}

	/**
	 * \brief Determines the optimal vertex position for an edge contraction.
	 * \param vertex_id The ID to use for the newly created vertex.
	 * \param edge01 The half-edge to evaluate.
	 * \param quadrics A mapping of error quadrics by vertex.
	 * \return The optimal vertex and cost associated with collapsing \p edge01.
	 */
	std::pair<std::shared_ptr<geometry::Vertex>, float> GetOptimalEdgeContractionVertex(
		const std::size_t vertex_id,
		const geometry::HalfEdge& edge01,
		const std::unordered_map<std::size_t, glm::mat4>& quadrics) {

		const auto v0 = edge01.Flip()->Vertex();
		const auto v1 = edge01.Vertex();

		const auto& q0 = quadrics.at(v0->Id());
		const auto& q1 = quadrics.at(v1->Id());
		const auto q01 = q0 + q1;

		const glm::mat3 Q{q01};
		const glm::vec3 b = glm::column(q01, 3);
		const auto d = q01[3][3];

		// if the upper 3x3 matrix of the error quadric is not invertible, average the edge vertices
		if (std::abs(glm::determinant(Q)) < epsilon || std::abs(d) < epsilon) {
			return {AverageVertices(vertex_id, *v0, *v1), 0.f};
		}

		const auto Q_inv = glm::inverse(Q);
		const auto D_inv = glm::column(glm::mat4{Q_inv}, 3, glm::vec4{-1.f / d * Q_inv * b, 1.f / d});

		auto position = D_inv * glm::vec4{0.f, 0.f, 0.f, 1.f};
		position /= position.w;
		const auto normal = (v0->Normal() + v1->Normal()) / 2.f;
		const auto cost = glm::dot(position, q01 * position);

		return {std::make_shared<geometry::Vertex>(vertex_id, position, normal), cost};
	}

	/**
	 * \brief Determines if the removal of an edge will cause the mesh to degenerate.
	 * \param edge01 The half-edge to evaluate.
	 * \return \c true if the removal of \p edge01 will cause the mesh to degenerate, otherwise \c false.
	 */
	bool WillDegenerate(const std::shared_ptr<geometry::HalfEdge>& edge01) {
		const auto v0 = edge01->Flip()->Vertex();
		const auto v1_next = edge01->Next()->Vertex();
		const auto v0_next = edge01->Flip()->Next()->Vertex();
		std::unordered_map<std::size_t, std::shared_ptr<geometry::Vertex>> neighborhood;

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

	/** \brief Represents an edge contraction candidate. */
	struct EdgeContraction {

		explicit EdgeContraction(
			geometry::HalfEdgeMesh& mesh,
			const std::shared_ptr<geometry::HalfEdge>& edge,
			const std::unordered_map<std::size_t, glm::mat4>& quadrics) : edge{edge} {
			std::tie(vertex, cost) = GetOptimalEdgeContractionVertex(mesh.NextVertexId(), *edge, quadrics);
		}

		/** \brief The edge to be collapsed. */
		const std::shared_ptr<geometry::HalfEdge> edge;

		/** \brief The optimal vertex position that minimizes the cost of collapsing this edge. */
		std::shared_ptr<geometry::Vertex> vertex;

		/** \brief The associated cost of collapsing this edge. */
		float cost = std::numeric_limits<float>::infinity();

		/** \brief Indicates this edge contraction candidate is valid. See comment on priority queue invalidation. */
		bool valid = true;
	};
}

gfx::Mesh geometry::mesh::Simplify(const gfx::Mesh& mesh, const float rate) {

	if (rate < 0.f || rate > 1.f) throw std::invalid_argument{std::format("Invalid mesh simplification rate {}", rate)};

	const auto start_time = std::chrono::high_resolution_clock::now();
	HalfEdgeMesh half_edge_mesh{mesh};

	// compute error quadrics for each vertex
	std::unordered_map<std::size_t, glm::mat4> quadrics;
	for (const auto& [vertex_id, vertex] : half_edge_mesh.Vertices()) {
		quadrics.emplace(vertex_id, ComputeQuadric(*vertex));
	}

	// use a priority queue to sort edge contraction candidates by the associate cost of collapsing that edge
	constexpr auto comparator = [](
		const std::shared_ptr<EdgeContraction>& lhs,
		const std::shared_ptr<EdgeContraction>& rhs) { return lhs->cost > rhs->cost; };
	std::priority_queue<
		std::shared_ptr<EdgeContraction>,
		std::vector<std::shared_ptr<EdgeContraction>>,
		decltype(comparator)> edge_contractions{comparator};

	// to work around the limitation of std::priority_queue not providing an API to update an existing entry's priority,
	// duplicate entries may be inserted and this map will be used to invalidate previous entries in the priority queue
	std::unordered_map<std::size_t, std::shared_ptr<EdgeContraction>> valid_edges;

	// compute the optimal vertex position that minimizes the cost of collapsing each edge
	for (const auto& edge : half_edge_mesh.Edges() | std::views::values) {
		const auto min_edge = GetMinEdge(edge);
		if (const auto min_edge_key = hash_value(*min_edge); !valid_edges.contains(min_edge_key)) {
			const auto edge_contraction = std::make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
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

		if (const auto & edge01 = edge_contraction->edge; edge_contraction->valid && !WillDegenerate(edge01)) {
			const auto& v_new = edge_contraction->vertex;
			const auto v0 = edge01->Flip()->Vertex();
			const auto v1 = edge01->Vertex();

			// remove the edge from the mesh and attach incident edges to the new vertex
			half_edge_mesh.CollapseEdge(edge01, v_new);

			// compute the error quadric for the new vertex
			const auto& q0 = quadrics.at(v0->Id());
			const auto& q1 = quadrics.at(v1->Id());
			quadrics.emplace(v_new->Id(), q0 + q1);

			// invalidate entries in the priority queue that were removed during the previous edge contraction
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

			// add new edge contraction candidates for edges affected by the previous edge contraction
			std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> visited_edges;
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
							std::make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
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

	const auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << std::format(
		"Mesh simplified from {} to {} triangles in {} seconds\n",
		initial_face_count,
		half_edge_mesh.Faces().size(),
		std::chrono::duration<float>{end_time - start_time}.count());

	return half_edge_mesh;
}
