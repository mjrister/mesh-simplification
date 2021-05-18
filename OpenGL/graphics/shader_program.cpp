#include "shader_program.h"

#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>

#include "utils/file.h"

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
	: id{glCreateShader(shader_type)} {

	if (!id) throw std::runtime_error{"Shader creation failed"};

	glShaderSource(id, 1, &shader_source, nullptr);
	glCompileShader(id);
	VerifyShaderStatus(id, GL_COMPILE_STATUS);
}

gfx::ShaderProgram::ShaderProgram(
	const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
	: id_{glCreateProgram()},
	  vertex_shader_{GL_VERTEX_SHADER, file::Read(vertex_shader_filepath).c_str()},
	  fragment_shader_{GL_FRAGMENT_SHADER, file::Read(fragment_shader_filepath).c_str()} {

	if (!id_) throw std::runtime_error{"Shader program creation failed"};

	glAttachShader(id_, vertex_shader_.id);
	glAttachShader(id_, fragment_shader_.id);

	glLinkProgram(id_);
	VerifyShaderProgramStatus(id_, GL_LINK_STATUS);

	glValidateProgram(id_);
	VerifyShaderProgramStatus(id_, GL_VALIDATE_STATUS);

	glDetachShader(id_, vertex_shader_.id);
	glDetachShader(id_, fragment_shader_.id);
}
