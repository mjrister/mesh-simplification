#pragma once

#include <memory>
#include <sstream>
#include <unordered_map>

#include "face.h"
#include "graphics/mesh.h"
#include "half_edge.h"
#include "vertex.h"

namespace geometry {

	class HalfEdgeMesh {

	public:
		explicit HalfEdgeMesh(const gfx::Mesh& mesh) {
			const auto& positions = mesh.Positions();
			const auto& normals = mesh.Normals();
			const auto& indices = mesh.Indices();

			vertices_by_id_.reserve(positions.size());
			for (std::size_t i = 0; i < positions.size(); ++i) {
				vertices_by_id_.emplace(i, std::make_shared<Vertex>(i, positions[i], normals[i]));
			}

			for (std::size_t i = 0; i < indices.size(); i += 3) {
				const auto v0 = vertices_by_id_[indices[i]];
				const auto v1 = vertices_by_id_[indices[i + 1]];
				const auto v2 = vertices_by_id_[indices[i + 2]];
				const auto face012 = CreateTriangle(v0, v1, v2);
				faces_by_id_.emplace(face012->Id(), face012);
			}
		}

		[[nodiscard]] const auto& VerticesById() const { return vertices_by_id_; }
		[[nodiscard]] const auto& EdgesById() const { return edges_by_id_; }
		[[nodiscard]] const auto& FacesById() const { return faces_by_id_; }

		gfx::Mesh ToMesh() {

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

		void CollapseEdge(const std::shared_ptr<HalfEdge>& edge01, const std::shared_ptr<Vertex>& v_new) {

			const auto edge10 = edge01->Flip();

			const auto v0 = edge10->Vertex();
			const auto v1 = edge01->Vertex();
			const auto vt = edge01->Next()->Vertex();
			const auto vb = edge10->Next()->Vertex();

			const auto face01t = edge01->Face();
			const auto face10b = edge10->Face();

			CollapseIncidentTriangles(v0, vt, vb, v_new);
			CollapseIncidentTriangles(v1, vb, vt, v_new);

			DeleteEdge(edge01);

			DeleteFace(face01t);
			DeleteFace(face10b);

			DeleteVertex(v0);
			DeleteVertex(v1);
		}

	private:
		std::shared_ptr<Face> CreateTriangle(
			const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) {

			const auto edge01 = CreateHalfEdge(v0, v1);
			const auto edge12 = CreateHalfEdge(v1, v2);
			const auto edge20 = CreateHalfEdge(v2, v0);

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

		std::shared_ptr<HalfEdge> CreateHalfEdge(
			const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1) {

			static auto get_or_insert = [this](const std::size_t id, const std::shared_ptr<Vertex>& vertex) {
				if (const auto iterator = edges_by_id_.find(id); iterator != edges_by_id_.end()) {
					return iterator->second;
				}
				const auto [iterator, _] = edges_by_id_.emplace(id, std::make_shared<HalfEdge>(id, vertex));
				return iterator->second;
			};

			const auto edge01_id = HalfEdge::GetHalfEdgeId(*v0, *v1);
			const auto edge01 = get_or_insert(edge01_id, v1);

			const auto edge10_id = HalfEdge::GetHalfEdgeId(*v1, *v0);
			const auto edge10 = get_or_insert(edge10_id, v0);

			edge01->SetFlip(edge10);
			edge10->SetFlip(edge01);

			return edge01;
		}

		void CollapseIncidentTriangles(
			const std::shared_ptr<Vertex>& v0,
			const std::shared_ptr<Vertex>& vt,
			const std::shared_ptr<Vertex>& vb,
			const std::shared_ptr<Vertex>& v_new) {

			auto vi = vt;
			std::shared_ptr<HalfEdge> edgej0;
			const auto edgeb0 = GetHalfEdge(vb, v0);

			do {
				const auto edge0i = GetHalfEdge(v0, vi);
				const auto edgeij = edge0i->Next();
				edgej0 = edgeij->Next();

				const auto vj = edgeij->Vertex();
				CreateTriangle(v_new, vi, vj);

				DeleteEdge(edge0i);
				DeleteFace(edge0i->Face());

				vi = vj;
			} while (edgej0 != edgeb0);

			DeleteEdge(edgeb0);
		}

		std::shared_ptr<HalfEdge> GetHalfEdge(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1) {
			const auto edge01_id = HalfEdge::GetHalfEdgeId(*v0, *v1);
			if (const auto iterator = edges_by_id_.find(edge01_id); iterator == edges_by_id_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent edge (" << v0->Id() << ',' << v1->Id() << ')';
				throw std::invalid_argument("Attempted to retrieve a nonexistent edge");
			} else {
				return iterator->second;
			}
		}

		void DeleteVertex(const std::shared_ptr<Vertex>& v0) {
			if (const auto iterator = vertices_by_id_.find(v0->Id()); iterator == vertices_by_id_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent vertex " << *v0;
				throw std::invalid_argument{oss.str()};
			} else {
				vertices_by_id_.erase(iterator);
			}
		}

		void DeleteEdge(const std::shared_ptr<HalfEdge>& edge01) {
			for (const auto& edge : {edge01, edge01->Flip()}) {
				if (const auto iterator = edges_by_id_.find(edge->Id()); iterator == edges_by_id_.end()) {
					std::ostringstream oss;
					oss << "Attempted to delete a nonexistent edge " << *edge01;
					throw std::invalid_argument{oss.str()};
				} else {
					edges_by_id_.erase(iterator);
				}
			}
		}

		void DeleteFace(const std::shared_ptr<Face>& face012) {
			if (const auto iterator = faces_by_id_.find(face012->Id()); iterator == faces_by_id_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent face " << *face012;
				throw std::invalid_argument{oss.str()};
			} else {
				faces_by_id_.erase(iterator);
			}
		}

		std::unordered_map<std::size_t, std::shared_ptr<Vertex>> vertices_by_id_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_by_id_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_by_id_;
	};
}
