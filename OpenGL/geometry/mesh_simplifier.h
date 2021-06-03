#pragma once

#include <algorithm>
#include <limits>
#include <queue>

#include <GL/gl3w.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "half_edge.h"
#include "half_edge_mesh.h"
#include "hash.h"

namespace geometry {

	class MeshSimplifier {

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
					a * d, b * d, c * d, d * d };
			}

			return quadric;
		}

		static std::pair<std::shared_ptr<Vertex>, GLfloat> GetOptimalEdgeContractionVertex(
			const HalfEdge& edge01,
			const std::unordered_map<std::size_t, const glm::mat4>& quadrics,
			const std::size_t next_vertex_id) {

			const auto v0 = edge01.Flip()->Vertex();
			const auto v1 = edge01.Vertex();

			const auto& K0 = quadrics.at(v0->Id());
			const auto& K1 = quadrics.at(v1->Id());
			const auto K01 = K0 + K1;

			glm::mat3 Q{K01};
			glm::vec3 b = glm::column(K01, 3);
			const auto d = K01[3][3];

			constexpr auto epsilon = std::numeric_limits<GLfloat>::epsilon();
			if (std::abs(glm::determinant(Q)) < epsilon || std::abs(d) < epsilon) {
				const auto v_new = Vertex::Average(next_vertex_id, *v0, *v1);
				return {std::make_shared<Vertex>(v_new), 0.f};
			}

			const auto Q_inv = inverse(Q);
			const auto D_inv = column(glm::mat4{Q_inv}, 3, glm::vec4{1.f / d * Q_inv * b, 1.f / d});

			auto position = D_inv * glm::vec4{0.f, 0.f, 0.f, 1.f};
			position /= position.w;
			const auto normal = (v0->Normal() + v1->Normal()) / 2.f;
			const auto cost = glm::dot(position, K01 * position);

			return {std::make_shared<Vertex>(next_vertex_id, position, normal), cost};
		}

		struct HalfEdgeEntry {

			HalfEdgeEntry(
				const std::shared_ptr<HalfEdge>& edge,
				const std::unordered_map<std::size_t, const glm::mat4>& quadrics,
				const std::size_t next_vertex_id)
				: edge{edge} {

				const auto vertex_and_cost = GetOptimalEdgeContractionVertex(*edge, quadrics, next_vertex_id);
				vertex = vertex_and_cost.first;
				cost = vertex_and_cost.second;
			}

			friend bool operator<(const HalfEdgeEntry& a, const HalfEdgeEntry& b) { return a.cost < b.cost; }

			std::shared_ptr<HalfEdge> edge;
			std::shared_ptr<Vertex> vertex;
			GLfloat cost;
			bool most_recent = true;
		};

		void Simplify(HalfEdgeMesh& mesh, const GLfloat stop_ratio) {

			std::unordered_map<std::size_t, glm::mat4> quadrics;
			for (const auto& [vertex_id, vertex] : mesh.Vertices()) {
				quadrics[vertex_id] = ComputeQuadric(*vertex);
			}

			std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> valid_edges;
			std::priority_queue<HalfEdgeEntry> edge_queue;

			for (const auto& [_, edge] : mesh.Edges()) {
				const auto min_edge = std::min<>(edge, edge->Flip(), [](const auto& a, const auto& b) {
					return a->Vertex()->Id() < b->Vertex()->Id();
				});
				if (const auto min_edge_key = hash_value(*min_edge); !valid_edges.count(min_edge_key)) {
					valid_edges.emplace(min_edge_key, min_edge);
					edge_queue.emplace(min_edge, quadrics, mesh.NextVertexId());
				}
			}

			const auto face_count = mesh.Faces().size();
			while (!edge_queue.empty() && face_count * stop_ratio > mesh.Faces().size()) {
			}
		}
	};
}
