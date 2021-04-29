#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace utils {

	static std::string ReadFile(const std::string_view shader_filepath) {

		if (std::ifstream ifs{shader_filepath.data()}; ifs.good()) {
			std::string source;
			ifs.seekg(0, std::ios::end);
			source.reserve(static_cast<std::size_t>(ifs.tellg()));
			ifs.seekg(0, std::ios::beg);
			source.assign(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});
			return source;
		}

		std::ostringstream oss;
		oss << "Unable to open " << shader_filepath;
		throw std::runtime_error{oss.str()};
	}
}
