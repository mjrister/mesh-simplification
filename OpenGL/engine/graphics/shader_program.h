#pragma once

#include <iostream>
#include <map>
#include <string_view>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace gfx {

	class ShaderProgram {

		class Shader {

		public:
			Shader(GLenum shader_type, const GLchar* shader_source);
			~Shader() { glDeleteShader(id); }

			Shader(const Shader&) = delete;
			Shader& operator=(const Shader&) = delete;

			Shader(Shader&&) noexcept = delete;
			Shader& operator=(Shader&&) noexcept = delete;

			const GLuint id;
		};

	public:
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);
		~ShaderProgram() { glDeleteProgram(id_); }

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		void Enable() const noexcept { glUseProgram(id_); }

		template <typename T>
		void SetUniform(const std::string_view name, const T& value) {
			if constexpr (const auto location = GetUniformLocation(name); std::is_same<T, GLfloat>::value) {
				glUniform1f(location, value);
			} else if constexpr (std::is_same<T, glm::vec3>::value) {
				glUniform3fv(location, 1, glm::value_ptr(value));
			} else if constexpr (std::is_same<T, glm::mat3>::value) {
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
			} else if constexpr (std::is_same<T, glm::mat4>::value) {
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
			}
		}

	private:
		[[nodiscard]] GLint GetUniformLocation(const std::string_view name) {
			if (const auto iterator = uniform_locations_.find(name); iterator == uniform_locations_.end()) {
				const auto location = glGetUniformLocation(id_, name.data());
				if (location == -1) {
					std::cerr << name << " is not an active uniform variable" << std::endl;
				}
				return uniform_locations_[std::string{name}] = location;
			} else {
				return iterator->second;
			}
		}

		const GLuint id_;
		const Shader vertex_shader_, fragment_shader_;
		std::map<std::string, GLint, std::less<>> uniform_locations_;
	};
}
