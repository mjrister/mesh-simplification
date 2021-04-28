#pragma once

#include <fstream>
#include <sstream>
#include <string_view>

class ObjectLoader {

public:
	static void LoadMesh(const std::string_view object_filepath) {

		if (std::ifstream ifs{object_filepath.data()}; ifs.good()) {
			LoadMesh(ifs);
			return;
		}

		std::ostringstream oss;
		oss << "Unable to open " << object_filepath;
		throw std::runtime_error{oss.str()};
	}

	static void LoadMesh(std::istream& is) {

		for (std::string line; std::getline(is, line);) {

		}
	}
};
