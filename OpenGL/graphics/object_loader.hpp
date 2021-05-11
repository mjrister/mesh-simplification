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

namespace gfx {

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
			std::vector<std::array<glm::ivec3, 3>> faces;

			for (std::string line; std::getline(is, line);) {
				if (line = string::Trim(line); !line.empty() && !string::StartsWith(line, "#")) {
					if (string::StartsWith(line, "v ")) {
						positions.push_back(ParseLine<GLfloat, 3>(line));
					} else if (string::StartsWith(line, "vt ")) {
						texture_coordinates.push_back(ParseLine<GLfloat, 2>(line));
					} else if (string::StartsWith(line, "vn ")) {
						normals.push_back(ParseLine<GLfloat, 3>(line));
					} else if (string::StartsWith(line, "f ")) {
						faces.push_back(ParseFace(line));
					}
				}
			}

			if (faces.empty()) return Mesh{positions, texture_coordinates, normals};

			std::vector<glm::vec3> ordered_positions;
			std::vector<glm::vec2> ordered_texture_coordinates;
			std::vector<glm::vec3> ordered_normals;

			const std::size_t indices = faces.size() * 3;
			ordered_positions.reserve(indices);
			ordered_texture_coordinates.reserve(texture_coordinates.empty() ? 0 : indices);
			ordered_normals.reserve(normals.empty() ? 0 : indices);

			for (const auto& face : faces) {
				for (const auto& index_group : face) {
					const auto position_index = index_group[0];
					ordered_positions.push_back(positions.at(position_index));

					if (const auto texture_coordinate_index = index_group[1]; texture_coordinate_index != npos_index_) {
						ordered_texture_coordinates.push_back(texture_coordinates.at(texture_coordinate_index));
					}
					if (const auto normal_index = index_group[2]; normal_index != npos_index_) {
						ordered_normals.push_back(normals.at(normal_index));
					}
				}
			}

			return Mesh{ordered_positions, ordered_texture_coordinates, ordered_normals};
		}

	private:
		template <typename T, std::uint8_t N>
		static glm::vec<N, T> ParseLine(const std::string_view line) {
			if (const auto tokens = string::Split(line, " \t"); tokens.size() == N + 1) {
				glm::vec<N, T> vec{};
				for (std::uint8_t i = 0; i < N; ++i) {
					vec[i] = ParseToken<T>(tokens[i + static_cast<std::uint8_t>(1)]);
				}
				return vec;
			}
			std::ostringstream oss;
			oss << "Unsupported format " << line;
			throw std::invalid_argument{oss.str()};
		}

		template <typename T>
		static T ParseToken(const std::string_view token_t) {
			T value;
			if (const auto [_, error_code] = std::from_chars(token_t.data(), token_t.data() + token_t.size(), value);
				error_code == std::errc::invalid_argument) {
				std::ostringstream oss;
				oss << "Unable to convert " << token_t << " to type " << typeid(T).name();
				throw std::invalid_argument{oss.str()};
			}
			return value;
		}

		static std::array<glm::ivec3, 3> ParseFace(const std::string_view line) {
			if (const auto tokens = string::Split(line, " \t"); tokens.size() == 4) {
				return {ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
			}
			std::ostringstream oss;
			oss << "Unsupported format " << line;
			throw std::invalid_argument{oss.str()};
		}

		static glm::ivec3 ParseIndexGroup(const std::string_view line) {
			static constexpr auto delimiter = "/";
			const auto tokens = string::Split(line, delimiter);

			switch (std::count(line.begin(), line.end(), *delimiter)) {
				case 0:
					if (tokens.size() == 1) {
						const auto x = ParseToken<GLint>(tokens[0]) - 1;
						return {x, npos_index_, npos_index_};
					}
					break;
				case 1:
					if (tokens.size() == 2) {
						const auto x = ParseToken<GLint>(tokens[0]) - 1;
						const auto y = ParseToken<GLint>(tokens[1]) - 1;
						return {x, y, npos_index_};
					}
					break;
				case 2:
					if (tokens.size() == 2 && *line.cbegin() != '/' && *(line.cend() - 1) != '/') {
						const auto x = ParseToken<GLint>(tokens[0]) - 1;
						const auto z = ParseToken<GLint>(tokens[1]) - 1;
						return {x, npos_index_, z};
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

		static constexpr GLint npos_index_{-1};
	};
}
