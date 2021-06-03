#pragma once

#include <algorithm>
#include <limits>
#include <queue>

#include <glm/gtc/matrix_access.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "half_edge.h"
#include "half_edge_mesh.h"
#include "hash.h"

namespace geometry {

	class MeshSimplifier {

		static constexpr float epsilon = std::numeric_limits<float>::epsilon();

		static glm::mat4 ComputeQuadric(const Vertex& vertex) {
			glm::mat4 quadric{0.f};

			for (auto edge = vertex.Edge()->Next()->Flip(); edge != vertex.Edge(); edge = edge->Next()->Flip()) {
				const auto& position = vertex.Position();
				const auto& normal = edge->Face()->Normal();
				const auto a = normal.x;
				const auto b = normal.y;
				const auto c = normal.z;
				const auto d = -glm::dot(position, normal);
				quadric += glm::mat4{
					a * a, b * a, c * a, d * a,
					a * b, b * b, c * b, d * b,
					a * c, b * c, c * c, d * c,
					a * d, b * d, c * d, d * d};
			}

			return quadric;
		}

		static std::pair<std::shared_ptr<Vertex>, float> GetOptimalEdgeContractionVertex(
			const std::size_t vertex_id,
			const HalfEdge& edge01,
			const std::unordered_map<std::size_t, glm::mat4>& quadrics) {

			const auto v0 = edge01.Flip()->Vertex();
			const auto v1 = edge01.Vertex();

			const auto& K0 = quadrics.at(v0->Id());
			const auto& K1 = quadrics.at(v1->Id());
			const auto K01 = K0 + K1;

			const glm::mat3 Q{K01};
			const glm::vec3 b = glm::column(K01, 3);
			const auto d = K01[3][3];

			if (std::abs(glm::determinant(Q)) < epsilon || std::abs(d) < epsilon) {
				const auto v_new = Vertex::Average(vertex_id, *v0, *v1);
				return {std::make_shared<Vertex>(v_new), 0.f};
			}

			const auto Q_inv = glm::inverse(Q);
			const auto D_inv = glm::column(glm::mat4{Q_inv}, 3, glm::vec4{1.f / d * Q_inv * b, 1.f / d});

			auto position = D_inv * glm::vec4{0.f, 0.f, 0.f, 1.f};
			position /= position.w;
			const auto normal = (v0->Normal() + v1->Normal()) / 2.f;
			const auto cost = dot(position, K01 * position);

			return {std::make_shared<Vertex>(vertex_id, position, normal), cost};
		}

		struct HalfEdgeEntry {

			HalfEdgeEntry(
				const std::shared_ptr<HalfEdge>& edge,
				const std::unordered_map<std::size_t, glm::mat4>& quadrics,
				const std::size_t next_vertex_id) : edge{edge} {
				std::tie(vertex, cost) = GetOptimalEdgeContractionVertex(next_vertex_id, *edge, quadrics);
			}

			friend bool operator<(const HalfEdgeEntry& a, const HalfEdgeEntry& b) {
				return a.cost < b.cost;
			}

			std::shared_ptr<HalfEdge> edge;
			std::shared_ptr<Vertex> vertex;
			float cost = std::numeric_limits<float>::infinity();
			bool most_recent = true;
		};

		static std::shared_ptr<HalfEdge> GetMinEdge(const std::shared_ptr<HalfEdge>& edge) {
			return std::min<>(edge, edge->Flip(), [](const auto& edge01, const auto& edge10) {
				return edge01->Vertex()->Id() < edge10->Vertex()->Id();
			});
		}

		static void CollapseEdge(
			const HalfEdgeEntry& entry,
			HalfEdgeMesh& mesh,
			std::unordered_map<std::size_t, glm::mat4>& quadrics,
			std::unordered_map<std::size_t, std::shared_ptr<HalfEdgeEntry>>& valid_pairs,
			std::priority_queue<std::shared_ptr<HalfEdgeEntry>>& edge_queue) {

			const auto edge01 = entry.edge;
			const auto v0 = edge01->Flip()->Vertex();
			const auto v1 = edge01->Vertex();
			const auto v_new = entry.vertex;

			// remove valid pairs for edges that will be removed
			for (const auto& vertex : {v0, v1}) {
				for (auto edge = vertex->Edge()->Next()->Flip(); edge != vertex->Edge(); edge = edge->Next()->Flip()) {
					if (edge == edge01) continue;
					const auto edge_key = hash_value(*edge);
					valid_pairs.erase(edge_key);
				}
			}
			const auto edge01_key = hash_value(*edge01);
			valid_pairs.erase(edge01_key);

			// collapse edge
			mesh.CollapseEdge(edge01, v_new);

			// merge edge quadrics
			const auto& K0 = quadrics.at(v0->Id());
			const auto& K1 = quadrics.at(v1->Id());
			quadrics.emplace(v_new->Id(), K0 + K1);

			std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> visited;

			const auto vi = v_new;
			for (auto edgeji = vi->Edge()->Next()->Flip(); edgeji != vi->Edge(); edgeji = edgeji->Next()->Flip()) {
				const auto vj = edgeji->Flip()->Vertex();

				for (auto edgekj = vj->Edge()->Next()->Flip(); edgekj != vj->Edge(); edgekj = edgekj->Next()->Flip()) {
					const auto min_edge = GetMinEdge(edgekj);

					if (const auto min_edge_key = hash_value(*min_edge); !visited.count(min_edge_key)) {
						if (auto iterator = valid_pairs.find(min_edge_key); iterator != valid_pairs.end()) {
							iterator->second->most_recent = false;
						}
						const auto edge_entry = std::make_shared<HalfEdgeEntry>(min_edge, quadrics, mesh.NextVertexId());
						valid_pairs[min_edge_key] = edge_entry;
						edge_queue.push(edge_entry);
					}
				}
			}
		}

	public:
		static void Simplify(HalfEdgeMesh& mesh, const float stop_ratio) {

			// compute quadric for each vertex
			std::unordered_map<std::size_t, glm::mat4> quadrics;
			for (const auto& [vertex_id, vertex] : mesh.Vertices()) {
				quadrics.emplace(vertex_id, ComputeQuadric(*vertex));
			}

			std::unordered_map<std::size_t, std::shared_ptr<HalfEdgeEntry>> valid_pairs;
			std::priority_queue<std::shared_ptr<HalfEdgeEntry>> edge_queue;

			for (const auto& [_, edge] : mesh.Edges()) {
				const auto min_edge = std::min<>(edge, edge->Flip(), [](const auto& edge01, const auto& edge10) {
					return edge01->Vertex()->Id() < edge10->Vertex()->Id();
				});
				if (const auto min_edge_key = hash_value(*min_edge); !valid_pairs.count(min_edge_key)) {
					const auto edge_entry = std::make_shared<HalfEdgeEntry>(min_edge, quadrics, mesh.NextVertexId());
					valid_pairs.emplace(min_edge_key, edge_entry);
					edge_queue.push(edge_entry);
				}
			}

			const auto initial_face_count = static_cast<float>(mesh.Faces().size());
			const auto should_stop = [&]() {
				const auto face_count = static_cast<float>(mesh.Faces().size());
				return face_count < initial_face_count * stop_ratio;
			};

			while (!edge_queue.empty() && !should_stop()) {
				const auto& entry = edge_queue.top();
				edge_queue.pop();
			}
		}
	};
}
