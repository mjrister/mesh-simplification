#pragma once

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "utils/file.hpp"

namespace gfx {

	class ShaderProgram {

	public:
		ShaderProgram(const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
			: name_{glCreateProgram()},
			  vertex_shader_{GL_VERTEX_SHADER, file::Read(vertex_shader_filepath).c_str()},
			  fragment_shader_{GL_FRAGMENT_SHADER, file::Read(fragment_shader_filepath).c_str()} {

			if (!name_) throw std::runtime_error{"Shader program creation failed"};

			glAttachShader(name_, vertex_shader_.Name());
			glAttachShader(name_, fragment_shader_.Name());

			glLinkProgram(name_);
			VerifyStatus(GL_LINK_STATUS);

			glValidateProgram(name_);
			VerifyStatus(GL_VALIDATE_STATUS);

			glDetachShader(name_, vertex_shader_.Name());
			glDetachShader(name_, fragment_shader_.Name());
		}

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		~ShaderProgram() {
			glDeleteProgram(name_);
		}

		void Enable() const noexcept {
			glUseProgram(name_);
		}

		void SetUniform(const std::string_view name, const glm::mat4& value) const {
			if (const auto location = glGetUniformLocation(name_, name.data()); location != -1) {
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
			}
		}

	private:
		void VerifyStatus(const GLenum status_type) const {
			GLint success;
			glGetProgramiv(name_, status_type, &success);

			if (!success) {
				GLsizei log_length;
				glGetProgramiv(name_, GL_INFO_LOG_LENGTH, &log_length);
				std::vector<GLchar> info_log(log_length);
				glGetProgramInfoLog(name_, log_length, &log_length, info_log.data());
				throw std::runtime_error{info_log.data()};
			}
		}

		const GLuint name_;
		const Shader vertex_shader_, fragment_shader_;
	};
}
