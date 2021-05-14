#pragma once


#include <string_view>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/fwd.hpp>

#include "utils/string.hpp"

namespace gfx {

	class ShaderProgram {

		class Shader {

		public:
			Shader(GLenum shader_type, const GLchar* shader_source);
			~Shader() { glDeleteShader(id); }

			Shader(const Shader&) = delete;
			Shader(Shader&&) noexcept = delete;
			Shader& operator=(const Shader&) = delete;
			Shader& operator=(Shader&&) noexcept = delete;

			const GLuint id;
		};

	public:
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);
		~ShaderProgram() { glDeleteProgram(id_); }

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		void Enable() const noexcept { glUseProgram(id_); }

		void SetUniform(std::string_view name, GLfloat value);
		void SetUniform(std::string_view name, const glm::vec3& value);
		void SetUniform(std::string_view name, const glm::mat3& value);
		void SetUniform(std::string_view name, const glm::mat4& value);

	private:
		[[nodiscard]] GLint GetUniformLocation(std::string_view name);

		const GLuint id_;
		const Shader vertex_shader_, fragment_shader_;
		std::unordered_map<std::string, GLint, string::string_hash, std::equal_to<>> uniform_locations_;
	};
}
