#include "file.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
	std::string Read(std::istream& is) {
		std::string source;
		is.seekg(0, std::ios::end);
		source.reserve(static_cast<std::size_t>(is.tellg()));
		is.seekg(0, std::ios::beg);
		source.assign(std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{});
		return source;
	}
}

std::string file::Read(const std::string_view filepath) {
	if (std::ifstream ifs{filepath.data()}; ifs.good()) {
		return ::Read(ifs);
	}
	std::ostringstream oss;
	oss << "Unable to open " << filepath;
	throw std::invalid_argument{oss.str()};
}
