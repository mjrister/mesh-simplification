#include <vector>

#include <GL/gl3w.h>
#include <gtest/gtest.h>

#include "geometry/half_edge_mesh.cpp"
#include "graphics/mesh.h"

using namespace geometry;
using namespace gfx;

namespace {

	//void VerifyEdge(
	//	const std::shared_ptr<Vertex>& v0,
	//	const std::shared_ptr<Vertex>& v1,
	//	const std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>>& edges_by_id) {

	//	const auto edge01_id = HalfEdge::GetHalfEdgeId(*v0, *v1);
	//	const auto edge10_id = HalfEdge::GetHalfEdgeId(*v1, *v0);

	//	const auto edge01_map_entry = edges_by_id.find(edge01_id);
	//	const auto edge10_map_entry = edges_by_id.find(edge10_id);

	//	ASSERT_NE(edge01_map_entry, edges_by_id.end());
	//	ASSERT_NE(edge10_map_entry, edges_by_id.end());

	//	const auto edge01 = edge01_map_entry->second;
	//	const auto edge10 = edge10_map_entry->second;

	//	ASSERT_EQ(edge01, edge10->Flip());
	//	ASSERT_EQ(edge10, edge01->Flip());
	//}

	TEST(HalfEdgeMeshTest, TestCreateHalfEdgeMesh) {

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
			0, 2, 3, // t0
			0, 3, 1, // t1
			0, 1, 7, // t2
			0, 7, 8, // t3
			0, 8, 9, // t4
			0, 9, 2, // t5
			1, 3, 4, // t6
			1, 4, 5, // t7
			1, 5, 6, // t8
			1, 6, 7  // t9
		};

		const Mesh mesh{positions, {}, normals, indices};
		const HalfEdgeMesh half_edge_mesh{mesh};

		const auto& vertices_by_id = half_edge_mesh.VerticesById();
		const auto& edges_by_id = half_edge_mesh.EdgesById();
		const auto& faces_by_id = half_edge_mesh.FacesById();

		ASSERT_EQ(10, vertices_by_id.size());
		ASSERT_EQ(38, edges_by_id.size());
		ASSERT_EQ(10, faces_by_id.size());

		for (std::size_t i = 0; i < indices.size(); i += 3) {

			const auto v0 = *vertices_by_id.find(indices[i]);
			const auto v1 = *vertices_by_id.find(indices[i+1]);
			const auto v2 = *vertices_by_id.find(indices[i+2]);
		}
	}
}