#include "obj_loader.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "mesh.h"
#include "utils/string.h"

namespace {
	constexpr GLint npos_index{-1};

	template <typename T>
	T ParseToken(const std::string_view token) {
		T value;
		if (const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);
			error_code == std::errc::invalid_argument) {
			std::ostringstream oss;
			oss << "Unable to convert " << token << " to type " << typeid(T).name();
			throw std::invalid_argument{oss.str()};
		}
		return value;
	}

	template <typename T, std::uint8_t N>
	glm::vec<N, T> ParseLine(const std::string_view line) {
		if (const auto tokens = string::Split(line, " \t"); tokens.size() == N + 1) {
			glm::vec<N, T> vec{};
			for (std::uint8_t i = 0; i <= N; ++i) {
				vec[i] = ParseToken<T>(tokens[i + 1]);
			}
			return vec;
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}

	glm::ivec3 ParseIndexGroup(const std::string_view line) {
		static constexpr auto delimiter = "/";
		const auto tokens = string::Split(line, delimiter);

		switch (std::count(line.cbegin(), line.cend(), *delimiter)) {
			case 0:
				if (tokens.size() == 1) {
					const auto x = ParseToken<GLint>(tokens[0]) - 1;
					return {x, npos_index, npos_index};
				}
				break;
			case 1:
				if (tokens.size() == 2) {
					const auto x = ParseToken<GLint>(tokens[0]) - 1;
					const auto y = ParseToken<GLint>(tokens[1]) - 1;
					return {x, y, npos_index};
				}
				break;
			case 2:
				if (tokens.size() == 2 && *line.cbegin() != '/' && *(line.cend() - 1) != '/') {
					const auto x = ParseToken<GLint>(tokens[0]) - 1;
					const auto z = ParseToken<GLint>(tokens[1]) - 1;
					return {x, npos_index, z};
				}
				if (tokens.size() == 3) {
					const auto x = ParseToken<GLint>(tokens[0]) - 1;
					const auto y = ParseToken<GLint>(tokens[1]) - 1;
					const auto z = ParseToken<GLint>(tokens[2]) - 1;
					return {x, y, z};
				}
				break;
		}

		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}

	std::array<glm::ivec3, 3> ParseFace(const std::string_view line) {
		if (const auto tokens = string::Split(line, " \t"); tokens.size() == 4) {
			return {ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}
}

gfx::Mesh gfx::obj_loader::LoadMesh(const std::string_view filepath) {
	if (std::ifstream ifs{filepath.data()}; ifs.good()) {
		return LoadMesh(ifs);
	}
	std::ostringstream oss;
	oss << "Unable to open " << filepath;
	throw std::invalid_argument{oss.str()};
}

gfx::Mesh gfx::obj_loader::LoadMesh(std::istream& is) {
	std::vector<glm::vec4> positions;
	std::vector<glm::vec2> texture_coordinates;
	std::vector<glm::vec3> normals;
	std::vector<std::array<glm::ivec3, 3>> faces;

	for (std::string line; std::getline(is, line);) {
		if (const auto line_view = string::Trim(line); !line_view.empty() && !string::StartsWith(line_view, "#")) {
			if (string::StartsWith(line_view, "v ")) {
				positions.emplace_back(ParseLine<GLfloat, 3>(line), 1.0f);
			} else if (string::StartsWith(line_view, "vt ")) {
				texture_coordinates.push_back(ParseLine<GLfloat, 2>(line));
			} else if (string::StartsWith(line_view, "vn ")) {
				normals.push_back(ParseLine<GLfloat, 3>(line));
			} else if (string::StartsWith(line_view, "f ")) {
				faces.push_back(ParseFace(line));
			}
		}
	}

	if (faces.empty()) return Mesh{positions, texture_coordinates, normals};

	std::vector<glm::vec2> ordered_texture_coordinates(texture_coordinates.empty() ? 0 : positions.size());
	std::vector<glm::vec3> ordered_normals(normals.empty() ? 0 : positions.size());
	std::vector<GLuint> indices;
	indices.reserve(faces.size() * 3);

	for (const auto& face : faces) {
		for (const auto& index_group : face) {
			const auto position_index = index_group[0];
			indices.push_back(position_index);

			if (const auto texture_coordinate_index = index_group[1]; texture_coordinate_index != npos_index) {
				ordered_texture_coordinates.at(position_index) = texture_coordinates.at(texture_coordinate_index);
			}
			if (const auto normal_index = index_group[2]; normal_index != npos_index) {
				ordered_normals.at(position_index) = normals.at(normal_index);
			}
		}
	}

	return Mesh{positions, ordered_texture_coordinates, ordered_normals, indices};
}
