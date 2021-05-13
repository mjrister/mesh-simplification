#pragma once

#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>

namespace gfx {

	class Shader {

	public:
		Shader(const GLenum shader_type, const GLchar* const shader_source)
			: id_{glCreateShader(shader_type)} {

			if (!id_) throw std::runtime_error{"Shader creation failed"};

			glShaderSource(id_, 1, &shader_source, nullptr);
			glCompileShader(id_);
			VerifyStatus(GL_COMPILE_STATUS);
		}

		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = delete;

		~Shader() {
			glDeleteShader(id_);
		}

		[[nodiscard]] GLuint Name() const noexcept {
			return id_;
		}

	private:
		void VerifyStatus(const GLenum status_type) const {
			GLint success;
			glGetShaderiv(id_, status_type, &success);

			if (!success) {
				GLsizei log_length;
				glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &log_length);
				std::vector<GLchar> info_log(log_length);
				glGetShaderInfoLog(id_, log_length, &log_length, info_log.data());
				throw std::runtime_error{info_log.data()};
			}
		}

		const GLuint id_;
	};
}
