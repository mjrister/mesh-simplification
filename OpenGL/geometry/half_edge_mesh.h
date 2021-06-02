#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/vec3.hpp>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/hash.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"

namespace geometry {
	class HalfEdgeMesh {

	public:
		explicit HalfEdgeMesh(const gfx::Mesh& mesh) {
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
				const auto face012 = CreateTriangle(v0, v1, v2);
				faces_.emplace(hash_value(*face012), face012);
			}

			next_vertex_id_ = positions.size();
		}

		[[nodiscard]] const auto& Vertices() const { return vertices_; }
		[[nodiscard]] const auto& Edges() const { return edges_; }
		[[nodiscard]] const auto& Faces() const { return faces_; }
		[[nodiscard]] std::size_t NextVertexId() { return next_vertex_id_++; }

		operator gfx::Mesh() const {

			std::vector<glm::vec3> positions;
			positions.reserve(vertices_.size());

			std::vector<glm::vec3> normals;
			normals.reserve(vertices_.size());

			std::vector<GLuint> indices;
			indices.reserve(faces_.size() * 3);

			GLuint i = 0;
			std::unordered_map<std::size_t, GLuint> index_map;

			for (const auto& [_, vertex] : vertices_) {
				index_map.emplace(vertex->Id(), i++);
				positions.push_back(vertex->Position());
				normals.push_back(vertex->Normal());
			}

			for (const auto& [_, face] : faces_) {
				indices.push_back(index_map.at(face->V0()->Id()));
				indices.push_back(index_map.at(face->V1()->Id()));
				indices.push_back(index_map.at(face->V2()->Id()));
			}

			return gfx::Mesh{positions, {}, normals, indices};
		}

		void CollapseEdge(
			const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v_new) {

			const auto edge01 = GetHalfEdge(*v0, *v1);
			const auto edge10 = edge01->Flip();

			const auto v_top = edge01->Next()->Vertex();
			const auto v_bottom = edge10->Next()->Vertex();

			CollapseIncidentTriangles(v0, v_top, v_bottom, v_new);
			CollapseIncidentTriangles(v1, v_bottom, v_top, v_new);

			DeleteEdge(*edge01);

			DeleteFace(*edge01->Face());
			DeleteFace(*edge10->Face());

			DeleteVertex(*v0);
			DeleteVertex(*v1);

			vertices_.emplace(v_new->Id(), v_new);
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

		std::shared_ptr<HalfEdge> CreateHalfEdge(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1) {

			const auto edge01_key = hash_value(*v0, *v1);
			const auto edge10_key = hash_value(*v1, *v0);

			if (const auto iterator = edges_.find(edge01_key); iterator != edges_.end()) {
				return iterator->second;
			}

			auto edge01 = std::make_shared<HalfEdge>(v0, v1);
			const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

			edge01->SetFlip(edge10);
			edge10->SetFlip(edge01);

			edges_.emplace(edge01_key, edge01);
			edges_.emplace(edge10_key, edge10);

			return edge01;
		}

		void CollapseIncidentTriangles(
			const std::shared_ptr<Vertex>& v_target,
			const std::shared_ptr<Vertex>& v_start,
			const std::shared_ptr<Vertex>& v_end,
			const std::shared_ptr<Vertex>& v_new) {

			const auto edge_start = GetHalfEdge(*v_target, *v_start);
			const auto edge_end = GetHalfEdge(*v_target, *v_end);

			for (auto edge0i = edge_start; edge0i != edge_end;) {

				const auto edgeij = edge0i->Next();
				const auto edgej0 = edgeij->Next();

				const auto vi = edge0i->Vertex();
				const auto vj = edgeij->Vertex();

				const auto face_new = CreateTriangle(v_new, vi, vj);
				faces_.emplace(hash_value(*face_new), face_new);

				DeleteEdge(*edge0i);
				DeleteFace(*edge0i->Face());

				edge0i = edgej0->Flip();
			}

			DeleteEdge(*edge_end);
		}

		std::shared_ptr<HalfEdge> GetHalfEdge(const Vertex& v0, const Vertex& v1) {

			if (const auto iterator = edges_.find(hash_value(v0, v1)); iterator == edges_.end()) {
				std::ostringstream oss;
				oss << "Attempted to retrieve a nonexistent edge (" << v0.Id() << ',' << v1.Id() << ')';
				throw std::invalid_argument("Attempted to retrieve a nonexistent edge");
			} else {
				return iterator->second;
			}
		}

		void DeleteVertex(const Vertex& vertex) {

			if (const auto iterator = vertices_.find(vertex.Id()); iterator == vertices_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent vertex " << vertex;
				throw std::invalid_argument{oss.str()};
			} else {
				vertices_.erase(iterator);
			}
		}

		void DeleteEdge(const HalfEdge& edge) {

			for (const auto& edge_key : {hash_value(edge), hash_value(*edge.Flip())}) {
				if (const auto iterator = edges_.find(edge_key); iterator == edges_.end()) {
					std::ostringstream oss;
					oss << "Attempted to delete a nonexistent edge " << edge;
					throw std::invalid_argument{oss.str()};
				} else {
					edges_.erase(iterator);
				}
			}
		}

		void DeleteFace(const Face& face) {

			if (const auto iterator = faces_.find(hash_value(face)); iterator == faces_.end()) {
				std::ostringstream oss;
				oss << "Attempted to delete a nonexistent face " << face;
				throw std::invalid_argument{oss.str()};
			} else {
				faces_.erase(iterator);
			}
		}

		std::map<std::size_t, std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
		std::size_t next_vertex_id_;
	};
}
