#pragma once

#include <array>
#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/gl3w.h>

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
		std::vector<GLfloat> positions, normals, texture_coordinates;

		for (std::string line; std::getline(is, line);) {
			if (line.empty() || string::StartsWith(line, "#")) continue;
			if (string::StartsWith(line, "v ")) {
				const auto position = ParseLine<GLfloat, 3>(line);
				positions.insert(positions.cend(), position.cbegin(), position.cend());
			} else if (string::StartsWith(line, "vn ")) {
				const auto normal = ParseLine<GLfloat, 3>(line);
				normals.insert(normals.cend(), normal.cbegin(), normal.cend());
			} else if (string::StartsWith(line, "vt ")) {
				const auto texture_coordinate = ParseLine<GLfloat, 2>(line);
				texture_coordinates.insert(
					texture_coordinates.cend(), texture_coordinate.cbegin(), texture_coordinate.cend());
			}
		}

		return Mesh{{}, {}};
	}

private:
	template <typename T, std::size_t N>
	static std::array<T, N> ParseLine(const std::string_view line) {
		if (const auto tokens = string::Split(line, " "); tokens.size() == N + 1) {
			std::array<T, N> data{};
			for (std::size_t i = 0; i < N; ++i) {
				data[i] = ParseToken<T>(tokens[i + 1]);
			}
			return data;
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
};
