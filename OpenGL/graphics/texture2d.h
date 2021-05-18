#pragma once

#include <string_view>

#include <GL/gl3w.h>

namespace gfx {

	class Texture2d {

	public:
		explicit Texture2d(std::string_view filepath, std::uint8_t texture_unit_index = 0);
		~Texture2d();

		Texture2d(const Texture2d&) = delete;
		Texture2d(Texture2d&&) noexcept = delete;
		Texture2d& operator=(const Texture2d&) = delete;
		Texture2d& operator=(Texture2d&&) noexcept = delete;

		void Bind() const noexcept;

	private:
		GLuint id_{};
		const std::uint8_t texture_unit_index_;
	};
}
