#include "graphics/obj_loader.cpp"  // NOLINT

#include <gtest/gtest.h>

namespace {

using namespace gfx;  // NOLINT

TEST(StringTest, TestTrimWhitespaceString) {
  static constexpr auto* kLine = "     ";
  static_assert(Trim(kLine).empty());
}

TEST(StringTest, TestTrimString) {
  static constexpr auto* kLine = "\t  Hello, World!  \t";
  static_assert("Hello, World!" == Trim(kLine));
}

TEST(StringTest, TestSplitEmptyString) {
  static constexpr auto* kLine = "";
  const auto tokens = Split(kLine, " ");
  EXPECT_TRUE(tokens.empty());
}

TEST(StringTest, TestSplitWhitespaceString) {
  static constexpr auto* kLine = "   ";
  const auto tokens = Split(kLine, " ");
  EXPECT_TRUE(tokens.empty());
}

TEST(StringTest, TestSplitNoWhitespaceString) {
  static constexpr auto* kLine = "Hello";
  const auto tokens = Split(kLine, " ");
  EXPECT_EQ((std::vector<std::string_view>{kLine}), tokens);
}

TEST(StringTest, TestSplitStringOnWhitespaceAndTab) {
  static constexpr auto* kLine = "\t vt 0.707 0.395 0.684 ";
  const auto tokens = Split(kLine, " \t");
  EXPECT_EQ((std::vector<std::string_view>{"vt", "0.707", "0.395", "0.684"}), tokens);
}

TEST(ObjLoaderTest, TestParseEmptyToken) { EXPECT_THROW(ParseToken<GLint>(""), std::invalid_argument); }

TEST(ObjLoaderTest, TestParseInvalidToken) {
  EXPECT_THROW(ParseToken<GLfloat>("Definitely a float"), std::invalid_argument);
}

TEST(ObjLoaderTest, TestParseIntToken) { EXPECT_EQ(42, ParseToken<GLint>("42")); }

TEST(ObjLoaderTest, TestParseFloatToken) { EXPECT_FLOAT_EQ(3.14f, ParseToken<GLfloat>("3.14")); }

TEST(ObjLoaderTest, TestParseEmptyLine) { EXPECT_THROW((ParseLine<GLfloat, 3>("")), std::invalid_argument); }

TEST(ObjLoaderTest, TestParseLineWithInvalidSizeArgument) {
  EXPECT_THROW((ParseLine<GLfloat, 2>("vt 0.707 0.395 0.684")), std::invalid_argument);
}

TEST(ObjLoaderTest, TestParseLine) {
  EXPECT_EQ((glm::vec3{.707f, .395f, .684f}), (ParseLine<GLfloat, 3>("vt 0.707 0.395 0.684")));
}

TEST(ObjLoaderTest, TestParseIndexGroupWithPositionIndex) {
  EXPECT_EQ((glm::ivec3{0, kInvalidFaceElementIndex, kInvalidFaceElementIndex}), ParseIndexGroup("1"));
}

TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
  EXPECT_EQ((glm::ivec3{0, 1, kInvalidFaceElementIndex}), ParseIndexGroup("1/2"));
}

TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
  EXPECT_EQ((glm::ivec3{0, kInvalidFaceElementIndex, 1}), ParseIndexGroup("1//2"));
}

TEST(ObjLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
  EXPECT_EQ((glm::ivec3{0, 1, 2}), ParseIndexGroup("1/2/3"));
}

TEST(ObjLoaderTest, TestParseInvalidIndexGroup) {
  EXPECT_THROW(ParseIndexGroup(""), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("//"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1//"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("//3"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1/2/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2/3"), std::invalid_argument);
}

TEST(ObjLoaderTest, TestParseFaceWithInvalidNumberOfIndexGroups) {
  EXPECT_THROW(ParseFace("f 1/2/3 4/5/6"), std::invalid_argument);
  EXPECT_THROW(ParseFace("f 1/2/3 4/5/6 7/8/9 10/11/12"), std::invalid_argument);
}

TEST(ObjLoaderTest, TestParseFaceWithThreeIndexGroups) {
  EXPECT_EQ((std::array{glm::ivec3{0, 1, 2}, glm::ivec3{3, 4, 5}, glm::ivec3{6, 7, 8}}),
            ParseFace("f 1/2/3 4/5/6 7/8/9"));
}

TEST(ObjLoaderTest, TestLoadMeshWithoutFaceIndices) {
  // clang-format off
  std::istringstream ss{R"(
    # positions
    v 0.0 0.1 0.2
    v 1.0 1.1 1.2
    v 2.0 2.1 2.2
    # texture coordinates
    vt 3.0 3.1
    vt 4.0 4.1
    vt 5.0 5.1
    # normals
    vn 6.0 6.1 6.2
    vn 7.0 7.1 7.2
    vn 8.0 8.1 8.2
  )"};
  // clang-format on

  const auto mesh = LoadMesh(ss);
  static constexpr glm::vec3 kV0{0.0f, 0.1f, 0.2f}, kV1{1.0f, 1.1f, 1.2f}, kV2{2.0f, 2.1f, 2.2f};
  static constexpr glm::vec2 kVt0{3.0f, 3.1f}, kVt1{4.0f, 4.1f}, kVt2{5.0f, 5.1f};
  static constexpr glm::vec3 kVn0{6.0f, 6.1f, 6.2f}, kVn1{7.0f, 7.1f, 7.2f}, kVn2{8.0f, 8.1f, 8.2f};

  EXPECT_EQ((std::vector{kV0, kV1, kV2}), mesh.positions());
  EXPECT_EQ((std::vector{kVt0, kVt1, kVt2}), mesh.texture_coordinates());
  EXPECT_EQ((std::vector{kVn0, kVn1, kVn2}), mesh.normals());
  EXPECT_TRUE(mesh.indices().empty());
}

TEST(ObjLoaderTest, TestLoadMeshWithFaceIndices) {
  // clang-format off
  std::istringstream ss{R"(
    # positions
    v 0.0 0.1 0.2
    v 1.0 1.1 1.2
    v 2.0 2.1 2.2
    v 3.0 3.1 3.2
    # texture coordinates
    vt 4.0 4.1
    vt 5.0 5.1
    vt 6.0 6.1
    vt 7.0 7.1
    # normals
    vn 8.0  8.1  8.2
    vn 9.0  9.1  9.2
    vn 10.0 10.1 10.2
    # faces
    f 1/4/2 2/1/3 3/2/1
    f 1/2/2 2/1/3 4/3/1
  )"};
  // clang-format on

  const auto mesh = LoadMesh(ss);
  static constexpr glm::vec3 kV0{0.0f, 0.1f, 0.2f}, kV1{1.0f, 1.1f, 1.2f}, kV2{2.0f, 2.1f, 2.2f}, kV3{3.0f, 3.1f, 3.2f};
  static constexpr glm::vec2 kVt0{4.0f, 4.1f}, kVt1{5.0f, 5.1f}, kVt2{6.0f, 6.1f}, kVt3{7.0f, 7.1f};
  static constexpr glm::vec3 kVn0{8.0f, 8.1f, 8.2f}, kVn1{9.0f, 9.1f, 9.2f}, kVn2{10.0f, 10.1f, 10.2f};

  EXPECT_EQ((std::vector{kV0, kV1, kV2, kV0, kV3}), mesh.positions());
  EXPECT_EQ((std::vector{kVt3, kVt0, kVt1, kVt1, kVt2}), mesh.texture_coordinates());
  EXPECT_EQ((std::vector{kVn1, kVn2, kVn0, kVn1, kVn0}), mesh.normals());
  EXPECT_EQ((std::vector{0u, 1u, 2u, 3u, 1u, 4u}), mesh.indices());
}

}  // namespace
