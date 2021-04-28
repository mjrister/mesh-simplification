#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <sstream>
#include <string>
#include <vector>

#include <GL/gl3w.h>

class ObjectLoader {

public:
	static void LoadString(std::istream& is) {
		std::vector<GLfloat> positions, normals, texture_coordinates;

		for (std::string line; std::getline(is, line);) {
			if (line.empty() || StartsWith(line, "#")) continue;
			if (StartsWith(line, "v ")) {
				const auto position = ParseLine<GLfloat, 3>(line);
				positions.insert(positions.cend(), position.cbegin(), position.cend());
			}
			else if (StartsWith(line, "vn ")) {
				const auto normal = ParseLine<GLfloat, 3>(line);
				normals.insert(normals.cend(), normal.cbegin(), normal.cend());
			}
			else if (StartsWith(line, "vt ")) {
				const auto texture_coordinate = ParseLine<GLfloat, 2>(line);
				texture_coordinates.insert(
					texture_coordinates.cend(), texture_coordinate.cbegin(), texture_coordinate.cend());
			}
		}
	}

private:
	static constexpr bool StartsWith(const std::string_view line, const std::string_view prefix) {
		return line.compare(0, prefix.size(), prefix.data()) == 0;
	}

	template <typename T, std::size_t N>
	static std::array<T, N> ParseLine(const std::string_view line) {
		if (const auto tokens = Split(line); tokens.size() == N) {
			std::array<T, N> data{};
			for (std::size_t i = 0; i < N; ++i) {
				data[i] = ParseToken<T>(tokens[i]);
			}
			return data;
		}
		std::ostringstream oss;
		oss << "Unsupported format " << line;
		throw std::runtime_error{oss.str()};
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
