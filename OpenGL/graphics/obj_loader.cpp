#include "graphics/obj_loader.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "graphics/mesh.h"
#include "utils/string.h"

namespace {

	// sentinel value indicating an unspecified face element index position
	constexpr GLint npos_index = -1;

	/**
	 * \brief Parses a string token.
	 * \tparam T The type to convert to.
	 * \param token The token to parse.
	 * \return The converted value of \p token to type \p T.
	 * \throw std::invalid_argument Indicates the string conversion failed.
	 */
	template <typename T>
	T ParseToken(const std::string_view token) {
		T value;
		if (const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);
			error_code == std::errc::invalid_argument) {
			throw std::invalid_argument{std::format("Unable to convert {} to type {}", token, typeid(T).name())};
		}
		return value;
	}

	/**
	 * \brief Parses a line in an .obj file.
	 * \tparam T The type to convert to.
	 * \tparam N The number of items to convert (does not include the first token identifying the line type).
	 * \param line The line to parse.
	 * \return A vector of size \p N containing each item in \p line converted to type \p T.
	 * \throw std::invalid_argument Indicates if the line format is unsupported.
	 */
	template <typename T, std::uint8_t N>
	glm::vec<N, T> ParseLine(const std::string_view line) {
		if (const auto tokens = string::Split(line, " \t"); tokens.size() == N + 1) {
			glm::vec<N, T> vec{};
			for (std::uint8_t i = 1; i <= N; ++i) {
				vec[i - 1] = ParseToken<T>(tokens[i]);
			}
			return vec;
		}
		throw std::invalid_argument{std::format("Unsupported format {}", line)};
	}

	/**
	 * \brief Parses a token representing a face element index group.
	 * \param token The token to parse. May optionally contain texture coordinate and normal indices.
	 * \return A vector containing vertex position, texture coordinate, and normal indices. Unspecified texture
	 *         coordinate and normal values are indicated by the value \c npos_index.
	 * \throw std::invalid_argument Indicates the index group format is unsupported.
	 */
	glm::ivec3 ParseIndexGroup(const std::string_view token) {
		static constexpr auto delimiter = "/";
		const auto tokens = string::Split(token, delimiter);

		switch (std::ranges::count(token, *delimiter)) {
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
				if (tokens.size() == 2 && *token.cbegin() != '/' && *(token.cend() - 1) != '/') {
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

		throw std::invalid_argument{std::format("Unsupported format {}", token)};
	}

	/**
	 * \brief Parses a line representing a triangular face element.
	 * \param line The line to parse.
	 * \return An array containing three parsed index groups for the face.
	 * \throw std::invalid_argument Indicates the line format is unsupported.
	 */
	std::array<glm::ivec3, 3> ParseFace(const std::string_view line) {
		if (const auto tokens = string::Split(line, " \t"); tokens.size() == 4) {
			return {ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
		}
		throw std::invalid_argument{std::format("Unsupported format {}", line)};
	}

	/**
	 * \brief Loads a triangle mesh from an input stream representing the contents of an .obj file.
	 * \param is The input stream to parse.
	 * \return A mesh defined by the position, texture coordinates, normals, and indices specified in the input stream.
	 * \throw std::invalid_argument Indicates the input stream contains an unsupported format.
	 */
	gfx::Mesh LoadMesh(std::istream& is) {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texture_coordinates;
		std::vector<glm::vec3> normals;
		std::vector<std::array<glm::ivec3, 3>> faces;

		for (std::string line; std::getline(is, line);) {
			if (line = string::Trim(line); !line.empty() && !line.starts_with('#')) {
				if (line.starts_with("v ")) {
					positions.push_back(ParseLine<GLfloat, 3>(line));
				} else if (line.starts_with("vt ")) {
					texture_coordinates.push_back(ParseLine<GLfloat, 2>(line));
				} else if (line.starts_with("vn ")) {
					normals.push_back(ParseLine<GLfloat, 3>(line));
				} else if (line.starts_with("f ")) {
					faces.push_back(ParseFace(line));
				}
			}
		}

		if (faces.empty()) return gfx::Mesh{positions, texture_coordinates, normals};

		std::vector<glm::vec3> ordered_positions;
		std::vector<glm::vec2> ordered_texture_coordinates;
		std::vector<glm::vec3> ordered_normals;
		std::vector<GLuint> indices;
		indices.reserve(faces.size() * 3);

		// For each index group, store texture coordinate and normals at the same index as the vertex position so that
		// data is aligned when sent to the vertex shader. Occasionally, index groups may specify different texture
		// coordinates or normals for the same vertex position. To handle this situation, an unordered map is used to
		// keep track of unique index groups and appends new position, texture coordinate, and normal triples to the end
		// of each respective ordered array as necessary.
		for (std::unordered_map<glm::ivec3, GLuint> unique_index_groups; const auto& face : faces) {
			for (const auto& index_group : face) {
				if (const auto iterator = unique_index_groups.find(index_group); iterator == unique_index_groups.end()) {
					const auto position_index = index_group[0];
					ordered_positions.push_back(positions.at(position_index));
					if (const auto texture_coordinate_index = index_group[1]; texture_coordinate_index != npos_index) {
						ordered_texture_coordinates.push_back(texture_coordinates.at(texture_coordinate_index));
					}
					if (const auto normal_index = index_group[2]; normal_index != npos_index) {
						ordered_normals.push_back(normals.at(normal_index));
					}
					const auto index = static_cast<GLuint>(ordered_positions.size()) - 1u;
					indices.push_back(index);
					unique_index_groups.emplace(index_group, index);
				} else {
					indices.push_back(iterator->second);
				}
			}
		}

		return gfx::Mesh{ordered_positions, ordered_texture_coordinates, ordered_normals, indices};
	}
}

gfx::Mesh gfx::obj_loader::LoadMesh(const std::string_view filepath) {
	if (std::ifstream ifs{filepath.data()}; ifs.good()) {
		return ::LoadMesh(ifs);
	}
	throw std::runtime_error{std::format("Unable to open {}", filepath)};
}
