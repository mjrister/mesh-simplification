#include <vector>

#include <GL/gl3w.h>
#include <gtest/gtest.h>

#include "geometry/half_edge_mesh.cpp"
#include "graphics/mesh.h"

using namespace geometry;
using namespace gfx;

namespace {
	std::shared_ptr<HalfEdge> MakeHalfEdge() {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{}, glm::vec3{});

		const auto edge01 = std::make_shared<HalfEdge>(v0, v1);
		const auto edge10 = std::make_shared<HalfEdge>(v1, v0);

		edge01->SetFlip(edge10);
		edge10->SetFlip(edge01);

		return edge01;
	}

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

		return Mesh{positions, {}, std::vector(10, glm::vec3{0.f, 0.f, 1.f}), indices};
	}

	HalfEdgeMesh MakeHalfEdgeMesh() {
		const auto mesh = MakeMesh();
		return HalfEdgeMesh{mesh};
	}

	void VerifyEdge(
		const Vertex& v0, const Vertex& v1, const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges) {

		const auto edge01_iterator = edges.find(hash_value(v0, v1));
		const auto edge10_iterator = edges.find(hash_value(v1, v0));

		ASSERT_NE(edge01_iterator, edges.end());
		ASSERT_NE(edge10_iterator, edges.end());

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
		const auto& vertices = half_edge_mesh.Vertices();
		const auto& edges = half_edge_mesh.Edges();
		const auto& faces = half_edge_mesh.Faces();

		for (std::size_t i = 0; i < indices.size(); i += 3) {

			const auto v0_iterator = vertices.find(indices[i]);
			const auto v1_iterator = vertices.find(indices[i + 1]);
			const auto v2_iterator = vertices.find(indices[i + 2]);

			ASSERT_NE(v0_iterator, vertices.end());
			ASSERT_NE(v1_iterator, vertices.end());
			ASSERT_NE(v2_iterator, vertices.end());

			const auto v0 = v0_iterator->second;
			const auto v1 = v1_iterator->second;
			const auto v2 = v2_iterator->second;

			VerifyEdge(*v0, *v1, edges);
			VerifyEdge(*v1, *v2, edges);
			VerifyEdge(*v2, *v0, edges);

			const auto edge01 = edges.at(hash_value(*v0, *v1));
			const auto edge12 = edges.at(hash_value(*v1, *v2));
			const auto edge20 = edges.at(hash_value(*v2, *v0));

			ASSERT_EQ(*edge01->Next(), *edge12);
			ASSERT_EQ(*edge12->Next(), *edge20);
			ASSERT_EQ(*edge20->Next(), *edge01);

			const auto face012_iterator = faces.find(hash_value(*v0, *v1, *v2));
			ASSERT_NE(face012_iterator, faces.end());

			const auto face012 = face012_iterator->second;
			ASSERT_EQ(*edge01->Face(), *face012);
			ASSERT_EQ(*edge12->Face(), *face012);
			ASSERT_EQ(*edge20->Face(), *face012);
			ASSERT_TRUE(*face012->Edge() == *edge01 || *face012->Edge() == *edge12 || *face012->Edge() == *edge20);
		}
	}

	TEST(HalfEdgeMeshTest, TestCreateHalfEdgeMesh) {

		const auto mesh = MakeMesh();
		const HalfEdgeMesh half_edge_mesh{mesh};

		ASSERT_EQ(10, half_edge_mesh.Vertices().size());
		ASSERT_EQ(38, half_edge_mesh.Edges().size());
		ASSERT_EQ(10, half_edge_mesh.Faces().size());

		VerifyTriangles(half_edge_mesh, mesh.Indices());
	}

	TEST(HalfEdgeMeshTest, TestHalfEdgeMeshConversionToMesh) {

		const auto mesh_a = MakeMesh();
		const Mesh mesh_b = HalfEdgeMesh{mesh_a};

		ASSERT_EQ(mesh_a.Positions(), mesh_b.Positions());
		ASSERT_EQ(mesh_a.TextureCoordinates(), mesh_b.TextureCoordinates());
		ASSERT_EQ(mesh_a.Normals(), mesh_b.Normals());
		ASSERT_EQ(mesh_a.Model(), mesh_b.Model());
	}

	TEST(HalfEdgeMeshTest, TestCollapseEdge) {

		auto half_edge_mesh = MakeHalfEdgeMesh();
		const auto& vertices = half_edge_mesh.Vertices();
		const auto& edges = half_edge_mesh.Edges();
		const auto v0 = vertices.at(0);
		const auto v1 = vertices.at(1);
		const auto edge01 = edges.at(hash_value(*v0, *v1));
		const auto v_new = Vertex::Average(half_edge_mesh.NextVertexId(), *v0, *v1);

		half_edge_mesh.CollapseEdge(edge01, std::make_shared<Vertex>(v_new));

		ASSERT_EQ(9, half_edge_mesh.Vertices().size());
		ASSERT_EQ(32, half_edge_mesh.Edges().size());
		ASSERT_EQ(8, half_edge_mesh.Faces().size());

		VerifyTriangles(half_edge_mesh, {
			2, 3, 10,
			3, 4, 10,
			4, 5, 10,
			5, 6, 10,
			6, 7, 10,
			7, 8, 10,
			8, 9, 10,
			2, 10, 9
		});
	}

	TEST(HalfEdgeMeshTest, TestGetNextVertexId) {
		auto half_edge_mesh = MakeHalfEdgeMesh();
		ASSERT_EQ(10, half_edge_mesh.NextVertexId());
		ASSERT_EQ(11, half_edge_mesh.NextVertexId());
		ASSERT_EQ(12, half_edge_mesh.NextVertexId());
	}

	TEST(HalfEdgeMeshTest, TestGetHalfEdge) {

		const auto edge01 = MakeHalfEdge();
		const auto edge10 = edge01->Flip();
		const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges{
			{hash_value(*edge01), edge01},
			{hash_value(*edge10), edge10}
		};

		const auto v0 = edge10->Vertex();
		const auto v1 = edge01->Vertex();
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{}, glm::vec3{});

		ASSERT_EQ(*edge01, *GetHalfEdge(*v0, *v1, edges));
		ASSERT_EQ(*edge10, *GetHalfEdge(*v1, *v0, edges));
		ASSERT_THROW(GetHalfEdge(*v0, *v2, edges), std::invalid_argument);
	}

	TEST(HalfEdgeMeshTest, TestDeleteVertex) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{}, glm::vec3{});
		std::map<std::size_t, std::shared_ptr<Vertex>> vertices{{0, v0}};

		DeleteVertex(*v0, vertices);

		ASSERT_TRUE(vertices.empty());
		ASSERT_THROW(DeleteVertex(*v0, vertices), std::invalid_argument);
	}

	TEST(HalfEdgeMeshTest, TestDeleteHalfEdge) {

		const auto edge01 = MakeHalfEdge();
		const auto edge10 = edge01->Flip();
		std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges{
			{hash_value(*edge01), edge01},
			{hash_value(*edge10), edge10}
		};

		DeleteEdge(*edge01, edges);

		ASSERT_TRUE(edges.empty());
		ASSERT_THROW(DeleteEdge(*edge01, edges), std::invalid_argument);
	}

	TEST(HalfEdgeMeshTest, TestDeleteFace) {

		const auto v0 = std::make_shared<Vertex>(0, glm::vec3{-1.f, -1.f, 0.f}, glm::vec3{});
		const auto v1 = std::make_shared<Vertex>(1, glm::vec3{0.f, .5f, 0.f}, glm::vec3{});
		const auto v2 = std::make_shared<Vertex>(2, glm::vec3{1.f, -1.f, 0.f}, glm::vec3{});
		const auto face012 = std::make_shared<Face>(v0, v1, v2);
		std::unordered_map<std::size_t, std::shared_ptr<Face>> faces{{hash_value(*face012), face012}};

		DeleteFace(*face012, faces);

		ASSERT_TRUE(faces.empty());
		ASSERT_THROW(DeleteFace(*face012, faces), std::invalid_argument);
	}
}