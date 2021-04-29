#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace file {

	static std::string Read(const std::string_view filepath) {

		if (std::ifstream ifs{filepath.data()}; ifs.good()) {
			std::string source;
			ifs.seekg(0, std::ios::end);
			source.reserve(static_cast<std::size_t>(ifs.tellg()));
			ifs.seekg(0, std::ios::beg);
			source.assign(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});
			return source;
		}

		std::ostringstream oss;
		oss << "Unable to open " << filepath;
		throw std::invalid_argument{oss.str()};
	}
}
