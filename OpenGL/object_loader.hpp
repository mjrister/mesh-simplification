#pragma once

#include <algorithm>
#include <charconv>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <GL/gl3w.h>

class ObjectLoader {

public:
	static void LoadString(std::istream& is) {
		std::vector<glm::vec3> positions, normals;
		std::vector<glm::vec2> texture_coordinates;

		for (std::string line; std::getline(is, line);) {
			if (line.empty() || StartsWith(line, "#")) {
				continue;
			}
			if (StartsWith(line, "v ")) {
				positions.push_back(ParseFloat3(line));
			} else if (StartsWith(line, "vn ")) {
				normals.push_back(ParseFloat3(line));
			} else if (StartsWith(line, "vt ")) {
				texture_coordinates.push_back(ParseFloat2(line));
			}
		}
	}

private:
	static constexpr bool StartsWith(const std::string_view line, const std::string_view prefix) {
		return line.compare(0, prefix.size(), prefix.data()) == 0;
	}

	static glm::vec3 ParseFloat3(const std::string_view line) {
		if (const auto values = ParseFloatData(line); values.size() == 3) {
			return glm::vec3{values[0], values[1], values[2]};
		}
		std::ostringstream oss;
		oss << "Unsupported position format " << line;
		throw std::runtime_error{oss.str()};
	}

	static glm::vec2 ParseFloat2(const std::string_view line) {
		if (const auto values = ParseFloatData(line); values.size() == 2) {
			return glm::vec2{values[0], values[1]};
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::runtime_error{oss.str()};
	}

	static std::vector<GLfloat> ParseFloatData(const std::string_view line) {
		const auto tokens = Split(line);
		std::vector<GLfloat> data;
		data.reserve(tokens.size() - 1);
		std::transform(tokens.cbegin() + 1, tokens.cend(), std::back_inserter(data), ParseToken<GLfloat>);
		return data;
	}

	static std::vector<std::string_view> Split(const std::string_view line) {
		std::vector<std::string_view> tokens;
		for (size_t i = 0, j = 0; j != std::string_view::npos; i = j + 1) {
			if (j = line.find_first_of(' ', i); i != j) {
				tokens.push_back(line.substr(i, j - i));
			}
		}
		return tokens;
	}

	template <typename T>
	static T ParseToken(const std::string_view token) {
		T value;
		if (const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);
			error_code == std::errc::invalid_argument) {
			std::ostringstream oss;
			oss << "An error occurred while attempting to parse " << token;
			throw std::runtime_error{oss.str()};
		}
		return value;
	}
};
