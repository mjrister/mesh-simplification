#include <vector>

#include <GL/gl3w.h>
#include <gtest/gtest.h>

#include "geometry/half_edge_mesh.cpp"
#include "graphics/mesh.h"

using namespace geometry;
using namespace gfx;

namespace {

	Mesh MakeMesh() {

		const std::vector<glm::vec3> positions{
			{1.f, 0.f, 0.f},   // v0
			{2.f, 0.f, 0.f},   // v1
			{.5f, -1.f, 0.f},  // v2
			{1.5f, -1.f, 0.f}, // v3
			{2.5f, -1.f, 0.f}, // v4
			{3.f, 0.f, 0.f},   // v5
			{2.5f, 1.f, 0.f},  // v6
			{1.5f, 1.f, 0.f},  // v7
			{.5f, 1.f, 0.f},   // v8
			{0.f, 0.f, 0.f}    // v9
		};

		const std::vector<glm::vec3> normals{
			{0.f, 0.f, 1.f}, // vn0
			{0.f, 0.f, 1.f}, // vn1
			{0.f, 0.f, 1.f}, // vn2
			{0.f, 0.f, 1.f}, // vn3
			{0.f, 0.f, 1.f}, // vn4
			{0.f, 0.f, 1.f}, // vn5
			{0.f, 0.f, 1.f}, // vn6
			{0.f, 0.f, 1.f}, // vn7
			{0.f, 0.f, 1.f}, // vn8
			{0.f, 0.f, 1.f}, // vn9
		};

		const std::vector<GLuint> indices{
			0, 2, 3, // f0
			0, 3, 1, // f1
			0, 1, 7, // f2
			0, 7, 8, // f3
			0, 8, 9, // f4
			0, 9, 2, // f5
			1, 3, 4, // f6
			1, 4, 5, // f7
			1, 5, 6, // f8
			1, 6, 7  // f9
		};

		return Mesh{positions, {}, normals, indices};
	}

	void VerifyEdge(
		const Vertex& v0,
		const Vertex& v1,
		const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges_by_id) {

		const auto edge01_iterator = edges_by_id.find(hash(v0, v1));
		const auto edge10_iterator = edges_by_id.find(hash(v1, v0));

		ASSERT_NE(edge01_iterator, edges_by_id.end());
		ASSERT_NE(edge10_iterator, edges_by_id.end());

		const auto edge01 = edge01_iterator->second;
		const auto edge10 = edge10_iterator->second;

		ASSERT_EQ(v0, *edge10->Vertex());
		ASSERT_EQ(v1, *edge01->Vertex());

		ASSERT_EQ(*edge01, *edge10->Flip());
		ASSERT_EQ(*edge10, *edge01->Flip());

		ASSERT_EQ(*edge01, *edge01->Flip()->Flip());
		ASSERT_EQ(*edge10, *edge10->Flip()->Flip());
	}

	void VerifyTriangles(const HalfEdgeMesh& half_edge_mesh, const std::vector<GLuint>& indices) {
		const auto& vertices_by_id = half_edge_mesh.Vertices();
		const auto& edges_by_id = half_edge_mesh.Edges();
		const auto& faces_by_id = half_edge_mesh.Faces();

		for (std::size_t i = 0; i < indices.size(); i += 3) {

			const auto v0_iterator = vertices_by_id.find(indices[i]);
			const auto v1_iterator = vertices_by_id.find(indices[i + 1]);
			const auto v2_iterator = vertices_by_id.find(indices[i + 2]);

			ASSERT_NE(v0_iterator, vertices_by_id.end());
			ASSERT_NE(v1_iterator, vertices_by_id.end());
			ASSERT_NE(v2_iterator, vertices_by_id.end());

			const auto v0 = v0_iterator->second;
			const auto v1 = v1_iterator->second;
			const auto v2 = v2_iterator->second;

			VerifyEdge(*v0, *v1, edges_by_id);
			VerifyEdge(*v1, *v2, edges_by_id);
			VerifyEdge(*v2, *v0, edges_by_id);

			const auto edge01 = edges_by_id.find(hash(*v0, *v1))->second;
			const auto edge12 = edges_by_id.find(hash(*v1, *v2))->second;
			const auto edge20 = edges_by_id.find(hash(*v2, *v0))->second;

			ASSERT_EQ(*edge01->Next(), *edge12);
			ASSERT_EQ(*edge12->Next(), *edge20);
			ASSERT_EQ(*edge20->Next(), *edge01);

			const auto face012_iterator = faces_by_id.find(hash(*v0, *v1, *v2));
			ASSERT_NE(face012_iterator, faces_by_id.end());

			const auto face012 = face012_iterator->second;
			ASSERT_EQ(*edge01->Face(), *face012);
			ASSERT_EQ(*edge12->Face(), *face012);
			ASSERT_EQ(*edge20->Face(), *face012);
			ASSERT_TRUE(*face012->Edge() == *edge01 || *face012->Edge() == *edge12 || *face012->Edge() == *edge20);
		}
	}

	TEST(HalfEdgeMeshTest, TestEqualVerticesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		ASSERT_EQ(hash(v0), hash(Vertex{v0}));
	}

	TEST(HalfEdgeMeshTest, TestEqualVertexPairsProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		ASSERT_EQ(hash(v0, v1), hash(Vertex{v0}, Vertex{v1}));
	}

	TEST(HalfEdgeMeshTest, TestEqualVertexTriplesProduceTheSameHashValue) {
		const Vertex v0{0, glm::vec3{0.f}, glm::vec3{1.f}};
		const Vertex v1{1, glm::vec3{2.f}, glm::vec3{3.f}};
		const Vertex v2{2, glm::vec3{4.f}, glm::vec3{5.f}};
		ASSERT_EQ(hash(v0, v1, v2), hash(Vertex{v0}, Vertex{v1}, Vertex{v2}));
	}

	TEST(HalfEdgeMeshTest, TestCreateHalfEdgeMesh) {

		const auto mesh = MakeMesh();
		HalfEdgeMesh half_edge_mesh{mesh};

		ASSERT_EQ(10, half_edge_mesh.Vertices().size());
		ASSERT_EQ(38, half_edge_mesh.Edges().size());
		ASSERT_EQ(10, half_edge_mesh.Faces().size());

		VerifyTriangles(half_edge_mesh, mesh.Indices());
	}

	TEST(HalfEdgeMeshTest, TestHalfEdgeMeshConversionToMesh) {

		const auto mesh_a = MakeMesh();
		const auto mesh_b = HalfEdgeMesh{mesh_a}.ToMesh();

		ASSERT_EQ(mesh_a.Positions(), mesh_b.Positions());
		ASSERT_EQ(mesh_a.TextureCoordinates(), mesh_b.TextureCoordinates());
		ASSERT_EQ(mesh_a.Normals(), mesh_b.Normals());
		ASSERT_EQ(mesh_a.Model(), mesh_b.Model());
	}
}