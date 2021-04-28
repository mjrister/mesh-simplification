#pragma once

#include <fstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <GL/gl3w.h>

class ShaderProgram {

public:
	ShaderProgram(const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
		: id_{glCreateProgram()},
		  vertex_shader_{GL_VERTEX_SHADER, ReadFile(vertex_shader_filepath).c_str()},
		  fragment_shader_{GL_FRAGMENT_SHADER, ReadFile(fragment_shader_filepath).c_str()} {

		glAttachShader(id_, vertex_shader_.id);
		glAttachShader(id_, fragment_shader_.id);

		glLinkProgram(id_);
		VerifyStatus(GL_LINK_STATUS);

		glValidateProgram(id_);
		VerifyStatus(GL_VALIDATE_STATUS);

		glDetachShader(id_, vertex_shader_.id);
		glDetachShader(id_, fragment_shader_.id);
	}

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&&) noexcept = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

	~ShaderProgram() {
		glDeleteProgram(id_);
	}

	void Enable() const noexcept {
		glUseProgram(id_);
	}

private:
	class Shader {

	public:
		Shader(const GLenum type, const GLchar* const shader_source) : id{glCreateShader(type)} {
			glShaderSource(id, 1, &shader_source, nullptr);
			glCompileShader(id);
			VerifyCompileStatus();
		}

		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = delete;

		~Shader() {
			glDeleteShader(id);
		}

		void VerifyCompileStatus() const {
			GLint success;
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);

			if (!success) {
				GLsizei log_length;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
				std::vector<GLchar> info_log(log_length);
				glGetShaderInfoLog(id, log_length, &log_length, info_log.data());
				throw std::runtime_error{info_log.data()};
			}
		}

		GLuint id;
	};

	static std::string ReadFile(const std::string_view filepath) {

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
		throw std::runtime_error{oss.str()};
	}

	void VerifyStatus(const GLenum type) const {
		GLint success;
		glGetProgramiv(id_, type, &success);

		if (!success) {
			GLsizei log_length;
			glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetProgramInfoLog(id_, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}

	GLuint id_;
	Shader vertex_shader_, fragment_shader_;
};
