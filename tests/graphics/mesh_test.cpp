#include "graphics/mesh.cpp"

#include <array>
#include <vector>

#include <gtest/gtest.h>

namespace {

using namespace gfx;

TEST(MeshTest, TestValidateMeshWithInvalidNumberOfPositions) {
  for (auto i = 0; i <= 4; ++i) {
    if (i != 3) {
      EXPECT_THROW((gfx::Mesh{std::vector<glm::vec3>(i), {}, {}, {}}), std::invalid_argument);
    }
  }
}

TEST(MeshTest, TestValidateMeshWithInvalidNumberOfTextureCoordinates) {
  const std::vector<glm::vec3> positions(3);
  for (auto i = 1; i <= 4; ++i) {
    if (i != 3) {
      EXPECT_THROW((gfx::Mesh{positions, std::vector<glm::vec2>(i), {}, {}}), std::invalid_argument);
    }
  }
}

TEST(MeshTest, TestValidateMeshInvalidNumberOfNormals) {
  const std::vector<glm::vec3> positions(3);
  for (auto i = 1; i <= 4; ++i) {
    if (i != 3) {
      EXPECT_THROW((gfx::Mesh{positions, {}, std::vector<glm::vec3>(i), {}}), std::invalid_argument);
    }
  }
}

TEST(MeshTest, TestValidateMeshWithInvalidIndices) {
  const std::vector<glm::vec3> positions(3);
  for (auto i = 1; i <= 4; ++i) {
    if (i != 3) {
      EXPECT_THROW((gfx::Mesh{positions, {}, {}, std::vector<GLuint>(i)}), std::invalid_argument);
    }
  }
}

TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesAndNormals) {
  const std::vector<glm::vec3> positions(3);
  const std::vector<glm::vec2> texture_coordinates(3);
  const std::vector<glm::vec3> normals(3);
  EXPECT_NO_THROW((gfx::Mesh{positions, texture_coordinates, normals, {}}));
}

TEST(MeshTest, TestValidateMeshWithCorrectNumberOfPositionsTextureCoordinatesNormalsAndIndices) {
  constexpr std::array<glm::vec3, 4> positions{};
  constexpr std::array<glm::vec2, 2> texture_coordinates{};
  constexpr std::array<glm::vec3, 5> normals{};
  constexpr std::array<GLuint, 3> indices{};
  EXPECT_NO_THROW((gfx::Mesh{positions, texture_coordinates, normals, indices}));
}
}  // namespace
