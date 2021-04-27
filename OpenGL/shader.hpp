#pragma once

#include <fstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <GL/gl3w.h>

class Shader {

public:
	Shader(const std::string_view filepath, const GLenum type) : id_{glCreateShader(type)} {
		const auto shader_source = ReadFile(filepath);
		const auto* const shader_source_cstr = shader_source.c_str();
		glShaderSource(id_, 1, &shader_source_cstr, nullptr);
		glCompileShader(id_);
		VerifyCompileStatus();
	}

	Shader(const Shader&) = delete;
	Shader(Shader&&) noexcept = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) noexcept = delete;

	~Shader() {
		glDeleteShader(id_);
	}

	[[nodiscard]] GLuint Id() const noexcept {
		return id_;
	}

private:
	static std::string ReadFile(const std::string_view filepath) {

		if (std::ifstream ifs{filepath.data()}; ifs.good()) {
			std::string source;
			ifs.seekg(0, std::ios::end);
			source.reserve(static_cast<std::size_t>(ifs.tellg()));
			ifs.seekg(0, std::ios::beg);
			source.assign(std::istreambuf_iterator{ifs}, std::istreambuf_iterator<char>{});
			return source;
		}

		std::ostringstream oss;
		oss << "Unable to open " << filepath;
		throw std::runtime_error{oss.str()};
	}

	void VerifyCompileStatus() const {
		GLint success;
		glGetShaderiv(id_, GL_COMPILE_STATUS, &success);

		if (!success) {
			GLsizei log_length{0};
			glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetShaderInfoLog(id_, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}

	GLuint id_{0};
};
