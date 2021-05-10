#pragma once

#include <sstream>
#include <string>

#include "graphics/mesh.h"

namespace gfx {
	namespace obj_loader {
		Mesh LoadMesh(std::string_view filepath);
		Mesh LoadMesh(std::istream& is);
	}
}
