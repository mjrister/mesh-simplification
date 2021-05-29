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

			vertices_.reserve(positions.size());
			for (std::size_t i = 0; i < positions.size(); ++i) {
				vertices_.emplace(i, std::make_shared<Vertex>(i, positions[i], normals[i]));
			}

			for (std::size_t i = 0; i < indices.size(); i += 3) {
				const auto v0 = vertices_[indices[i]];
				const auto v1 = vertices_[indices[i + 1]];
				const auto v2 = vertices_[indices[i + 2]];
				const auto face012_id = GetFaceId(*v0, *v1, *v2);
				const auto face012 = CreateTriangle(v0, v1, v2);
				faces_.emplace(face012_id, face012);
			}
		}

		gfx::Mesh ToMesh() {

			std::vector<glm::vec4> positions;
			positions.reserve(vertices_.size());

			std::vector<glm::vec3> normals;
			normals.reserve(vertices_.size());

			for (const auto& [_, vertex] : vertices_) {
				positions.push_back(vertex->Position());
				normals.push_back(vertex->Normal());
			}

			std::vector<GLuint> indices;
			indices.reserve(faces_.size() * 3);

			for (const auto& [_, face] : faces_) {
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
		static std::size_t GetHalfEdgeId(const Vertex& v0, const Vertex& v1) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
			return seed;
		}

		static std::size_t GetFaceId(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
			std::size_t seed = 0x1C2CB417;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + std::hash<std::uint64_t>{}(v0.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + std::hash<std::uint64_t>{}(v1.Id());
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + std::hash<std::uint64_t>{}(v2.Id());
			return seed;
		}

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

			static const auto& get_or_insert = [this](const std::size_t id, const std::shared_ptr<Vertex>& vertex) {
				if (const auto iterator = edges_.find(id); iterator != edges_.end()) {
					return iterator->second;
				}
				const auto [iterator, _] = edges_.emplace(id, std::make_shared<HalfEdge>(vertex));
				return iterator->second;
			};

			const auto edge01_id = GetHalfEdgeId(*v0, *v1);
			const auto edge01 = get_or_insert(edge01_id, v1);

			const auto edge10_id = GetHalfEdgeId(*v1, *v0);
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
			const auto edge01_id = GetHalfEdgeId(*v0, *v1);
			if (const auto iterator = edges_.find(edge01_id); iterator == edges_.end()) {
				std::ostringstream oss;
				oss << "Attempted to retrieve a nonexistent edge (" << *v0 << ',' << *v1 << ')' << std::endl;
				throw std::invalid_argument(oss.str());
			} else {
				return iterator->second;
			}
		}

		void DeleteVertex(const std::shared_ptr<Vertex>& v0) {
			if (const auto iterator = vertices_.find(v0->Id()); iterator == vertices_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent vertex " << *v0 << std::endl;
				throw std::invalid_argument{oss.str()};
			} else {
				vertices_.erase(iterator);
			}
		}

		void DeleteEdge(const std::shared_ptr<HalfEdge>& edge01) {
			for (const auto& edge : {edge01, edge01->Flip()}) {
				const auto edge_id = GetHalfEdgeId(*edge->Vertex(), *edge->Flip()->Vertex());
				if (const auto iterator = edges_.find(edge_id); iterator == edges_.end()) {
					std::ostringstream oss;
					oss << "Attempted to delete a nonexistent edge " << edge << std::endl;
					throw std::invalid_argument{oss.str()};
				} else {
					edges_.erase(iterator);
				}
			}
		}

		void DeleteFace(const std::shared_ptr<Face>& face012) {
			const auto face012_id = GetFaceId(*face012->V0(), *face012->V1(), *face012->V2());
			if (const auto iterator = faces_.find(face012_id); iterator == faces_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete nonexistent face " << *face012 << std::endl;
				throw std::invalid_argument{oss.str()};
			} else {
				faces_.erase(iterator);
			}
		}

		std::unordered_map<std::size_t, std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
	};
}
