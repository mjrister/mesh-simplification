#pragma once

#include <string_view>

#include <GL/gl3w.h>

#include "shader.hpp"

class ShaderProgram {

public:
	ShaderProgram(const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
		: id_{glCreateProgram()} {
		const Shader vertex_shader{vertex_shader_filepath, GL_VERTEX_SHADER};
		const Shader fragment_shader{fragment_shader_filepath, GL_FRAGMENT_SHADER};
		glAttachShader(id_, vertex_shader.Id());
		glAttachShader(id_, fragment_shader.Id());
		glLinkProgram(id_);
		VerifyLinkStatus();
		glDetachShader(id_, fragment_shader.Id());
		glDetachShader(id_, vertex_shader.Id());
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
	void VerifyLinkStatus() const {
		GLint success;
		glGetProgramiv(id_, GL_LINK_STATUS, &success);

		if (!success) {
			GLsizei log_length{0};
			glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetProgramInfoLog(id_, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}

	GLuint id_{0};
};
