#pragma once

#include <string_view>

namespace gfx {
	class Mesh;

	namespace obj_loader {

		/**
		 * \brief Loads a triangle mesh from an .obj file.
		 * \param filepath The filepath to the .obj file.
		 * \return A mesh defined by the position, texture coordinates, normals, and indices specified in the .obj file.
		 * \remark At this time, only a subset of .obj file specification is supported. Specifically, only 3D vertex
		 *         position, 2D texture coordinates, and 3D normals are supported. Face elements are supported and may
		 *         optionally contain texture coordinate and normal indices.
		* \see https://en.wikipedia.org/wiki/Wavefront_.obj_file
		 */
		Mesh LoadMesh(std::string_view filepath);
	}
}
