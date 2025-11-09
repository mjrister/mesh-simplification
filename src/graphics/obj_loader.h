#ifndef GRAPHICS_OBJ_LOADER_H_
#define GRAPHICS_OBJ_LOADER_H_

#include <filesystem>

namespace gfx {
class Mesh;

namespace obj_loader {

/**
 * @brief Loads a triangle mesh from an .obj file.
 * @param filepath The path to the .obj file.
 * @return A mesh defined by the position, texture coordinates, normals, and indices specified in the .obj file.
 * @throw std::invalid_argument Thrown if the file format is unsupported.
 * @throw std::runtime_error Thrown if the file cannot be opened.
 * @note At this time, only a subset of the .obj file specification is supported which includes 3D vertex positions,
 *       2D texture coordinates, and 3D normals. Face elements are supported and may optionally contain texture
 *       coordinates, normals, and indices.
 * @see https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */
Mesh LoadMesh(const std::filesystem::path& filepath);

}  // namespace obj_loader
}  // namespace gfx

#endif  // GRAPHICS_OBJ_LOADER_H_
