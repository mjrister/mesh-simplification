#pragma once

#include <string_view>

#include <GL/gl3w.h>

namespace gfx {

	class Texture2d {

	public:
		Texture2d(std::string_view filepath, GLenum texture_unit);
		~Texture2d();

		Texture2d(const Texture2d&) = delete;
		Texture2d(Texture2d&&) noexcept = delete;
		Texture2d& operator=(const Texture2d&) = delete;
		Texture2d& operator=(Texture2d&&) noexcept = delete;

		void Bind() const noexcept;

	private:
		GLuint name_{};
		const GLenum texture_unit_;
	};
}
