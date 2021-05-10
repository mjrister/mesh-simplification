#include "shader_program.h"

#include <stdexcept>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "utils/file.hpp"

namespace {
	void VerifyShaderStatus(const GLuint shader_id, const GLenum status_type) {
		GLint success;
		glGetShaderiv(shader_id, status_type, &success);

		if (!success) {
			GLsizei log_length;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetShaderInfoLog(shader_id, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}

	void VerifyShaderProgramStatus(const GLuint shader_program_id, const GLenum status_type) {
		GLint success;
		glGetProgramiv(shader_program_id, status_type, &success);

		if (!success) {
			GLsizei log_length;
			glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetProgramInfoLog(shader_program_id, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}
}

gfx::ShaderProgram::Shader::Shader(const GLenum shader_type, const GLchar* const shader_source)
	: name{glCreateShader(shader_type)} {

	if (!name) throw std::runtime_error{"Shader creation failed"};

	glShaderSource(name, 1, &shader_source, nullptr);
	glCompileShader(name);
	VerifyShaderStatus(name, GL_COMPILE_STATUS);
}

gfx::ShaderProgram::Shader::~Shader() {
	glDeleteShader(name);
}

gfx::ShaderProgram::ShaderProgram(
	const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
	: name_{glCreateProgram()},
	  vertex_shader_{GL_VERTEX_SHADER, file::Read(vertex_shader_filepath).c_str()},
	  fragment_shader_{GL_FRAGMENT_SHADER, file::Read(fragment_shader_filepath).c_str()} {

	if (!name_) throw std::runtime_error{"Shader program creation failed"};

	glAttachShader(name_, vertex_shader_.name);
	glAttachShader(name_, fragment_shader_.name);

	glLinkProgram(name_);
	VerifyShaderProgramStatus(name_, GL_LINK_STATUS);

	glValidateProgram(name_);
	VerifyShaderProgramStatus(name_, GL_VALIDATE_STATUS);

	glDetachShader(name_, vertex_shader_.name);
	glDetachShader(name_, fragment_shader_.name);
}

gfx::ShaderProgram::~ShaderProgram() {
	glDeleteProgram(name_);
}

void gfx::ShaderProgram::Enable() const noexcept {
	glUseProgram(name_);
}

void gfx::ShaderProgram::SetUniform(const std::string_view name, const glm::mat4& value) const {
	if (const auto location = glGetUniformLocation(name_, name.data()); location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
	}
}
