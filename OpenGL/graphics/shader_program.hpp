#pragma once

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "utils/file.hpp"

namespace gfx {

	class ShaderProgram {

	public:
		ShaderProgram(const std::string_view vertex_shader_filepath, const std::string_view fragment_shader_filepath)
			: id_{glCreateProgram()},
			  vertex_shader_{GL_VERTEX_SHADER, file::Read(vertex_shader_filepath).c_str()},
			  fragment_shader_{GL_FRAGMENT_SHADER, file::Read(fragment_shader_filepath).c_str()} {

			if (!id_) throw std::runtime_error{"Shader program creation failed"};

			glAttachShader(id_, vertex_shader_.Name());
			glAttachShader(id_, fragment_shader_.Name());

			glLinkProgram(id_);
			VerifyStatus(GL_LINK_STATUS);

			glValidateProgram(id_);
			VerifyStatus(GL_VALIDATE_STATUS);

			glDetachShader(id_, vertex_shader_.Name());
			glDetachShader(id_, fragment_shader_.Name());
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

		void SetUniform(const std::string_view name, const GLfloat value) {
			const auto location = GetUniformLocation(name);
			glUniform1f(location, value);
		}

		void SetUniform(const std::string_view name, const glm::vec3& value) {
			const auto location = GetUniformLocation(name);
			glUniform3fv(location, 1, glm::value_ptr(value));
		}

		void SetUniform(const std::string_view name, const glm::mat3& value) {
			const auto location = GetUniformLocation(name);
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
		}

		void SetUniform(const std::string_view name, const glm::mat4& value) {
			const auto location = GetUniformLocation(name);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
		}

	private:
		[[nodiscard]] GLint GetUniformLocation(const std::string_view name) {
			if (!uniform_locations_.contains(name)) {
				const auto location = glGetUniformLocation(id_, name.data());
				if (location == -1) {
					std::cerr << name << " is not an active uniform variable" << std::endl;
				}
				uniform_locations_[std::string{name}] = location;
			}
			return uniform_locations_.find(name)->second;
		}

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

		const GLuint id_;
		const Shader vertex_shader_, fragment_shader_;

		struct string_view_hash {
			using is_transparent = void;

			size_t operator()(const std::string_view value) const { return std::hash<std::string_view>{}(value); }
		};
		std::unordered_map<std::string, GLint, string_view_hash, std::equal_to<>> uniform_locations_;
	};
}
