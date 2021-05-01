#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "graphics/mesh.hpp"
#include "utils/string.hpp"

class ObjectLoader {
		friend class ObjectLoaderTest;

public:
	static Mesh LoadMesh(const std::string_view filepath) {
		if (std::ifstream ifs{filepath.data()}; ifs.good()) {
			return LoadMesh(ifs);
		}
		std::ostringstream oss;
		oss << "Unable to open " << filepath;
		throw std::invalid_argument{oss.str()};
	}

	static Mesh LoadMesh(std::istream& is) {
		std::vector<glm::vec3> positions, normals;
		std::vector<glm::vec2> texture_coordinates;
		std::vector<std::array<glm::uvec3, 3>> faces;

		for (std::string line; std::getline(is, line);) {
			if (line.empty() || string::StartsWith(line, "#")) continue;
			if (string::StartsWith(line, "v ")) {
				positions.push_back(ParseLine<GLfloat, 3>(line));
			} else if (string::StartsWith(line, "vn ")) {
				normals.push_back(ParseLine<GLfloat, 3>(line));
			} else if (string::StartsWith(line, "vt ")) {
				texture_coordinates.push_back(ParseLine<GLfloat, 2>(line));
			} else if (string::StartsWith(line, "f ")) {
				faces.push_back(ParseFace(line));
			}
		}

		std::vector<glm::vec3> ordered_normals(positions.size());
		std::vector<glm::vec2> ordered_texture_coordinates(positions.size());

		for (const auto& face : faces) {
			for (const auto& index_group : face) {
				const auto position_index = index_group.x;
				ValidateIndex(position_index, positions.size() - 1);

				if (const auto texture_coordinate_index = index_group.y; texture_coordinate_index != npos_index_) {
					ValidateIndex(texture_coordinate_index, texture_coordinates.size() - 1);
					ordered_texture_coordinates[position_index] = texture_coordinates[texture_coordinate_index];
				}
				if (const auto normal_index = index_group.z; normal_index != npos_index_) {
					ValidateIndex(normal_index, normals.size() - 1);
					ordered_normals[position_index] = normals[normal_index];
				}
			}
		}

		return Mesh{Flatten(positions), Flatten(texture_coordinates), Flatten(normals), GetPositionIndices(faces)};
	}

private:
	template <typename T, std::uint8_t N>
	static glm::vec<N,T> ParseLine(const std::string_view line) {
		if (const auto tokens = string::Split(line, " "); tokens.size() == N + 1) {
			glm::vec<N, T> vec{};
			for (std::uint8_t i = 0; i < N; ++i) {
				vec[i] = ParseToken<T>(tokens[i + 1]);
			}
			return vec;
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}

	template <typename T>
	static T ParseToken(const std::string_view token) {
		T value;
		if (const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);
			error_code == std::errc::invalid_argument) {
			std::ostringstream oss;
			oss << "Unable to convert " << token << " to type '" << typeid(T).name();
			throw std::invalid_argument{oss.str()};
		}
		return value;
	}

	static std::array<glm::uvec3, 3> ParseFace(const std::string_view line) {
		if (const auto tokens = string::Split(line, " "); tokens.size() == 4) {
			return {ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}

	static glm::uvec3 ParseIndexGroup(const std::string_view line) {
		static constexpr auto delimiter = "/";
		const auto tokens = string::Split(line, delimiter);
		const auto count = std::count(line.cbegin(), line.cend(), *delimiter);

		if (count == 0 && tokens.size() == 1) {
			return {ParseToken<GLint>(tokens[0]), npos_index_, npos_index_};
		}
		if (count == 1 && tokens.size() == 2) {
			return {ParseToken<GLint>(tokens[0]), ParseToken<GLint>(tokens[1]), npos_index_};
		}
		if (count == 2 && tokens.size() == 2 && *line.cbegin() != '/' && *(line.cend() - 1) != '/') {
			return {ParseToken<GLint>(tokens[0]), npos_index_, ParseToken<GLint>(tokens[1])};
		}
		if (count == 2 && tokens.size() == 3) {
			return {ParseToken<GLint>(tokens[0]), ParseToken<GLint>(tokens[1]), ParseToken<GLint>(tokens[2])};
		}

		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::invalid_argument{oss.str()};
	}

	static void ValidateIndex(const GLint index, const GLint max_value) {
		if (index != std::clamp(index, 0, max_value)) {
			std::ostringstream oss;
			oss << "Index out of bounds: " << index;
			throw std::invalid_argument{oss.str()};
		}
	}

	template <typename T, std::uint8_t N>
	static std::vector<T> Flatten(const std::vector<glm::vec<N, T>>& tuples) {
		std::vector<T> data;
		data.reserve(tuples.size() * N);

		for (const auto& tuple : tuples) {
			for (std::uint8_t i = 0; i < N; ++i) {
				data.push_back(tuple[i]);
			}
		}

		return data;
	}

	static std::vector<GLuint> GetPositionIndices(const std::vector<std::array<glm::uvec3, 3>>& faces) {
		std::vector<GLuint> position_indices;
		position_indices.reserve(faces.size() * 3);

		for (const auto& face : faces) {
			for (const auto& index_group : face) {
				position_indices.push_back(index_group[0]);
			}
		}

		return position_indices;
	}

	static constexpr GLuint npos_index_ = std::numeric_limits<GLuint>::max();
};
