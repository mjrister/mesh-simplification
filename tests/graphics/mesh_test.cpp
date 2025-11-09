#include "graphics/mesh.cpp"  // NOLINT

#include <array>
#include <vector>

#include <gtest/gtest.h>

namespace {

using namespace gfx;  // NOLINT

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
  static constexpr std::array<glm::vec3, 4> kPositions{};
  static constexpr std::array<glm::vec2, 2> kTextureCoordinates{};
  static constexpr std::array<glm::vec3, 5> kNormals{};
  static constexpr std::array<GLuint, 3> kIndices{};
  EXPECT_NO_THROW((gfx::Mesh{kPositions, kTextureCoordinates, kNormals, kIndices}));
}

}  // namespace
