#include "geometry/mesh_simplifier.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "geometry/half_edge.h"
#include "geometry/half_edge_mesh.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

namespace {
	std::shared_ptr<geometry::HalfEdge> GetMinEdge(const std::shared_ptr<geometry::HalfEdge>& edge) {
		return std::min<>(edge, edge->Flip(), [](const auto& edge01, const auto& edge10) {
			return edge01->Vertex()->Id() < edge10->Vertex()->Id();
		});
	}

	glm::mat4 ComputeQuadric(const geometry::Vertex& v0) {
		glm::mat4 quadric{0.f};
		const auto& position = v0.Position();
		auto iterator = v0.Edge();
		do {
			const auto& normal = iterator->Face()->Normal();
			const auto plane = glm::vec4{normal, -glm::dot(position, normal)};
			quadric += glm::outerProduct(plane, plane);
			iterator = iterator->Next()->Flip();
		} while (iterator != v0.Edge());
		return quadric;
	}

	std::pair<std::shared_ptr<geometry::Vertex>, float> GetEdgeContractionVertex(
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

		constexpr auto epsilon = std::numeric_limits<float>::epsilon();
		if (std::abs(glm::determinant(Q)) < epsilon || std::abs(d) < epsilon) {
			const auto v_new = geometry::Vertex::Average(vertex_id, *v0, *v1);
			return {std::make_shared<geometry::Vertex>(v_new), 0.f};
		}

		const auto Q_inv = glm::inverse(Q);
		const auto D_inv = glm::column(glm::mat4{Q_inv}, 3, glm::vec4{-1.f / d * Q_inv * b, 1.f / d});

		auto position = D_inv * glm::vec4{0.f, 0.f, 0.f, 1.f};
		position /= position.w;
		const auto normal = (v0->Normal() + v1->Normal()) / 2.f;
		const auto cost = glm::dot(position, q01 * position);

		return {std::make_shared<geometry::Vertex>(vertex_id, position, normal), cost};
	}

	bool WillDegenerate(const std::shared_ptr<geometry::HalfEdge>& edge) {
		const auto v0 = edge->Flip()->Vertex();
		const auto va = edge->Next()->Vertex();
		const auto vb = edge->Flip()->Next()->Vertex();
		std::unordered_map<std::size_t, std::shared_ptr<geometry::Vertex>> neighborhood;

		for (auto iterator = edge->Next(); iterator != edge->Flip(); iterator = iterator->Flip()->Next()) {
			if (const auto vertex = iterator->Vertex(); vertex != v0 && vertex != va && vertex != vb) {
				neighborhood.emplace(hash_value(*vertex), vertex);
			}
		}

		for (auto iterator = edge->Flip()->Next(); iterator != edge; iterator = iterator->Flip()->Next()) {
			if (const auto vertex = iterator->Vertex(); neighborhood.count(hash_value(*vertex))) {
				return true;
			}
		}

		return false;
	}

	struct EdgeContraction {

		explicit EdgeContraction(
			geometry::HalfEdgeMesh& mesh,
			const std::shared_ptr<geometry::HalfEdge>& edge,
			const std::unordered_map<std::size_t, glm::mat4>& quadrics) : edge{edge} {
			std::tie(vertex, cost) = GetEdgeContractionVertex(mesh.NextVertexId(), *edge, quadrics);
		}

		std::shared_ptr<geometry::HalfEdge> edge;
		std::shared_ptr<geometry::Vertex> vertex;
		float cost = std::numeric_limits<float>::infinity();
		bool valid = true;
	};
}

gfx::Mesh geometry::mesh_simplifier::Simplify(const gfx::Mesh& mesh, const float stop_ratio) {

	HalfEdgeMesh half_edge_mesh{mesh};

	std::unordered_map<std::size_t, glm::mat4> quadrics;
	for (const auto& [vertex_id, vertex] : half_edge_mesh.Vertices()) {
		quadrics.emplace(vertex_id, ComputeQuadric(*vertex));
	}

	constexpr auto comparator = [](
		const std::shared_ptr<EdgeContraction>& lhs,
		const std::shared_ptr<EdgeContraction>& rhs) { return lhs->cost > rhs->cost; };
	std::priority_queue<
		std::shared_ptr<EdgeContraction>,
		std::vector<std::shared_ptr<EdgeContraction>>,
		decltype(comparator)> edge_contractions{comparator};
	std::unordered_map<std::size_t, std::shared_ptr<EdgeContraction>> valid_edges;

	for (const auto& [_, edge] : half_edge_mesh.Edges()) {
		const auto min_edge = GetMinEdge(edge);
		if (const auto min_edge_key = hash_value(*min_edge); !valid_edges.count(min_edge_key)) {
			const auto edge_contraction = std::make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
			edge_contractions.push(edge_contraction);
			valid_edges.emplace(min_edge_key, edge_contraction);
		}
	}

	const auto initial_face_count = static_cast<float>(half_edge_mesh.Faces().size());
	const auto should_stop = [&]() {
		const auto face_count = static_cast<float>(half_edge_mesh.Faces().size());
		return face_count < initial_face_count * stop_ratio;
	};

	while (!edge_contractions.empty() && !should_stop()) {
		const auto& edge_contraction = edge_contractions.top();
		const auto& edge01 = edge_contraction->edge;
		const auto& v_new = edge_contraction->vertex;
		const auto v0 = edge01->Flip()->Vertex();
		const auto v1 = edge01->Vertex();

		if (edge_contraction->valid && !WillDegenerate(edge01)) {

			half_edge_mesh.CollapseEdge(edge01, v_new);

			const auto& q0 = quadrics.at(v0->Id());
			const auto& q1 = quadrics.at(v1->Id());
			quadrics.emplace(v_new->Id(), q0 + q1);

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

			std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> visited_edges;
			const auto& vi = v_new;
			auto edgeji = vi->Edge();
			do {
				const auto vj = edgeji->Flip()->Vertex();
				auto edgekj = vj->Edge();
				do {
					const auto min_edge = GetMinEdge(edgekj);
					if (const auto min_edge_key = hash_value(*min_edge); !visited_edges.count(min_edge_key)) {
						if (auto iterator = valid_edges.find(min_edge_key); iterator != valid_edges.end()) {
							iterator->second->valid = false;
						}
						const auto edge_contraction = std::make_shared<EdgeContraction>(half_edge_mesh, min_edge, quadrics);
						valid_edges[min_edge_key] = edge_contraction;
						edge_contractions.emplace(edge_contraction);
						visited_edges.emplace(min_edge_key, min_edge);
					}
					edgekj = edgekj->Next()->Flip();
				} while (edgekj != vj->Edge());
				edgeji = edgeji->Next()->Flip();
			} while (edgeji != vi->Edge());
		}

		edge_contractions.pop();
	}

	return half_edge_mesh;
}

