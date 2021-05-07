#pragma once

#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>

class Shader {

public:
	Shader(const GLenum shader_type, const GLchar* const shader_source) : id_{glCreateShader(shader_type)} {
		if (id_) {
			glShaderSource(id_, 1, &shader_source, nullptr);
			glCompileShader(id_);
			VerifyStatus();
		} else {
			throw std::runtime_error{"Shader creation failed"};
		}
	}

	Shader(const Shader&) = delete;
	Shader(Shader&&) noexcept = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) noexcept = delete;

	~Shader() {
		glDeleteShader(id_);
	}

	[[nodiscard]] GLuint Id() const noexcept {
		return id_;
	}

private:
	void VerifyStatus() const {
		GLint success;
		glGetShaderiv(id_, GL_COMPILE_STATUS, &success);

		if (!success) {
			GLsizei log_length;
			glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> info_log(log_length);
			glGetShaderInfoLog(id_, log_length, &log_length, info_log.data());
			throw std::runtime_error{info_log.data()};
		}
	}

	GLuint id_;
};
