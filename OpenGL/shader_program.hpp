#pragma once

#include <stdexcept>
#include <string_view>
#include <vector>

#include <GL/gl3w.h>

#include "shader.hpp"
#include "utils/file_utils.hpp"

class ShaderProgram {

public:
	ShaderProgram(const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
		: id_{glCreateProgram()},
		  vertex_shader_{GL_VERTEX_SHADER, file::Read(vertex_shader_filepath).c_str()},
		  fragment_shader_{GL_FRAGMENT_SHADER, file::Read(fragment_shader_filepath).c_str()} {

		glAttachShader(id_, vertex_shader_.Id());
		glAttachShader(id_, fragment_shader_.Id());

		glLinkProgram(id_);
		VerifyStatus(GL_LINK_STATUS);

		glValidateProgram(id_);
		VerifyStatus(GL_VALIDATE_STATUS);

		glDetachShader(id_, vertex_shader_.Id());
		glDetachShader(id_, fragment_shader_.Id());
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
	void VerifyStatus(const GLenum status_type) const {
		GLint success;
		glGetProgramiv(id_, status_type, &success);

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
