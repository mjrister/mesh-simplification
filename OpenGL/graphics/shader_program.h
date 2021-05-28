#pragma once

#include <iostream>
#include <map>
#include <string_view>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace gfx {

	/** \brief An abstraction for a program that will execute one or more OpenGL shaders on the host GPU. */
	class ShaderProgram {

		/** \brief Represents a shader in the OpenGL graphics pipeline. */
		class Shader {

		public:
			/**
			 * \brief Initializes a shader.
			 * \param shader_type The shader type (e.g., GL_FRAGMENT_SHADER)
			 * \param shader_source The shader source code to be compiled.
			 * \throw std::runtime_error Indicates shader creation was unsuccessful.
			 */
			Shader(GLenum shader_type, const GLchar* shader_source);
			~Shader() { glDeleteShader(id); }

			Shader(const Shader&) = delete;
			Shader& operator=(const Shader&) = delete;

			Shader(Shader&&) noexcept = delete;
			Shader& operator=(Shader&&) noexcept = delete;

			const GLuint id;
		};

	public:

		/**
		 * \brief Initializes a shader program.
		 * \param vertex_shader_filepath The filepath to the vertex shader to be compiled.
		 * \param fragment_shader_filepath The filepath to the fragment shader to be compiled.
		 * \throw std::runtime_error Indicates the shader program creation was unsuccessful.
		 */
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);
		~ShaderProgram() { glDeleteProgram(id_); }

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		/** \brief Enables this shader program for immediate use in rendering. */
		void Enable() const { glUseProgram(id_); }

		/**
		 * \brief Sets a uniform variable in the shader program.
		 * \tparam T The uniform variable type.
		 * \param name The uniform variable name.
		 * \param value The uniform variable value.
		 */
		template <typename T>
		void SetUniform(const std::string_view name, const T& value) {
			if constexpr (const auto location = GetUniformLocation(name); std::is_same<T, GLfloat>::value) {
				glUniform1f(location, value);
			} else if constexpr (std::is_same<T, glm::vec3>::value) {
				glUniform3fv(location, 1, glm::value_ptr(value));
			} else if constexpr (std::is_same<T, glm::vec4>::value) {
				glUniform4fv(location, 1, glm::value_ptr(value));
			} else if constexpr (std::is_same<T, glm::mat3>::value) {
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
			} else if constexpr (std::is_same<T, glm::mat4>::value) {
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
			} else {
				static_assert(false, "Unsupported uniform variable type");
			}
		}

	private:

		/**
		 * \brief Gets the location for a uniform variable in the shader program.
		 * \param name The uniform variable name.
		 * \return An integer representing the uniform variable location. Returns -1 if the variable is not active.
		 */
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

		// Ideally, this would be an unordered_map, however, as of C++17 heterogeneous lookup is only supported for
		// ordered containers. This is important because each uniform location query is performed using a string_view,
		// but stored as a string. Without heterogeneous lookup, each query would have to be converted to a string
		// (and hence allocate unnecessary memory) which would degrade performance on the critical rendering path.
		std::map<std::string, GLint, std::less<>> uniform_locations_;
	};
}
