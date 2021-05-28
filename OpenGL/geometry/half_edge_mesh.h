#pragma once

#include <memory>
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
				vertices_.push_back(std::make_shared<Vertex>(static_cast<GLuint>(i), positions[i], normals[i]));
			}

			for (std::size_t i = 0; i < indices.size(); i += 3) {
				const auto v0 = vertices_[indices[i]];
				const auto v1 = vertices_[indices[i + 1]];
				const auto v2 = vertices_[indices[i + 2]];

				const auto face012_key = hash_value(*v0, *v1, *v2);
				const auto face012 = CreateTriangle(v0, v1, v2);
				faces_.emplace(face012_key, face012);
			}
		}

		gfx::Mesh ToMesh() {

			std::vector<glm::vec4> positions;
			positions.reserve(vertices_.size());

			std::vector<glm::vec3> normals;
			normals.reserve(vertices_.size());

			for (const auto& vertex : vertices_) {
				positions.push_back(vertex->Position());
				normals.push_back(vertex->Normal());
			}

			std::vector<GLuint> indices;
			indices.reserve(faces_.size() * 3);

			for (const auto& [_, face] : faces_) {
				indices.push_back(face->V0()->Id());
				indices.push_back(face->V1()->Id());
				indices.push_back(face->V2()->Id());
			}

			return gfx::Mesh{positions, {}, normals, indices};
		}

	private:
		std::shared_ptr<Face> CreateTriangle(
			const std::shared_ptr<Vertex>& v0,
			const std::shared_ptr<Vertex>& v1,
			const std::shared_ptr<Vertex>& v2) {

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

			static const auto& get_or_insert = [this](const std::size_t key, const std::shared_ptr<Vertex>& vertex) {
				if (const auto iterator = edges_.find(key); iterator != edges_.end()) {
					return iterator->second;
				}
				const auto& [iterator, _] = edges_.emplace(key, std::make_shared<HalfEdge>(vertex));
				return iterator->second;
			};

			const auto edge01_key = hash_value(*v0, *v1);
			const auto& edge01 = get_or_insert(edge01_key, v1);

			const auto edge10_key = hash_value(*v1, *v0);
			const auto& edge10 = get_or_insert(edge10_key, v0);

			edge01->SetFlip(edge10);
			edge10->SetFlip(edge01);

			return edge01;
		}

		std::vector<std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
	};
}
