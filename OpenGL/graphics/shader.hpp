#pragma once

#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>

namespace gfx {

	class Shader {

	public:
		Shader(const GLenum shader_type, const GLchar* const shader_source)
			: name_{glCreateShader(shader_type)} {

			if (!name_) throw std::runtime_error{"Shader creation failed"};

			glShaderSource(name_, 1, &shader_source, nullptr);
			glCompileShader(name_);
			VerifyStatus();
		}

		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = delete;

		~Shader() {
			glDeleteShader(name_);
		}

		[[nodiscard]] GLuint Name() const noexcept {
			return name_;
		}

	private:
		void VerifyStatus() const {
			GLint success;
			glGetShaderiv(name_, GL_COMPILE_STATUS, &success);

			if (!success) {
				GLsizei log_length;
				glGetShaderiv(name_, GL_INFO_LOG_LENGTH, &log_length);
				std::vector<GLchar> info_log(log_length);
				glGetShaderInfoLog(name_, log_length, &log_length, info_log.data());
				throw std::runtime_error{info_log.data()};
			}
		}

		const GLuint name_;
	};
}
