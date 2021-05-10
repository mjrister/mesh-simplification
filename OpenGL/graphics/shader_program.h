#pragma once

#include <string_view>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace gfx {

	class ShaderProgram {

		class Shader {

		public:
			Shader(GLenum shader_type, const GLchar* shader_source);
			~Shader();

			Shader(const Shader&) = delete;
			Shader(Shader&&) noexcept = delete;
			Shader& operator=(const Shader&) = delete;
			Shader& operator=(Shader&&) noexcept = delete;

			const GLuint name;
		};

	public:
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);
		~ShaderProgram();

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		void Enable() const noexcept;
		void SetUniform(std::string_view name, const glm::mat4& value) const;

	private:
		const GLuint name_;
		const Shader vertex_shader_, fragment_shader_;
	};
}
