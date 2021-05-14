#pragma once


#include <string_view>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/fwd.hpp>

#include "shader.hpp"

namespace gfx {

	class ShaderProgram {

	public:
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);
		~ShaderProgram();

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		void Enable() const noexcept;

		void SetUniform(std::string_view name, GLfloat value);
		void SetUniform(std::string_view name, const glm::vec3& value);
		void SetUniform(std::string_view name, const glm::mat3& value);
		void SetUniform(std::string_view name, const glm::mat4& value);

	private:
		[[nodiscard]] GLint GetUniformLocation(std::string_view name);

		const GLuint id_;
		const Shader vertex_shader_, fragment_shader_;

		struct string_hash {
			using is_transparent = void;
			size_t operator()(const std::string_view value) const { return std::hash<std::string_view>{}(value); }
		};
		std::unordered_map<std::string, GLint, string_hash, std::equal_to<>> uniform_locations_;
	};
}
