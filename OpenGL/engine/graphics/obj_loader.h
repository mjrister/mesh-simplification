#pragma once


#include <istream>
#include <string_view>

namespace gfx {
	class Mesh;

	namespace obj_loader {
		Mesh LoadMesh(std::string_view filepath);
	}
}
