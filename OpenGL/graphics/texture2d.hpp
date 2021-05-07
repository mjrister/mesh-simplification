#pragma once

#include <sstream>
#include <stdexcept>
#include <string_view>

#include <GL/gl3w.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture2d {

public:
	explicit Texture2d(const std::string_view filepath)
		: texture_unit_{instance_count_++} {

		if (texture_unit_ >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
			std::ostringstream oss;
			oss << "Texture exceeds the maximum number of allowed texture units " << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1;
			throw std::invalid_argument{oss.str()};
		}

		glGenTextures(1, &id_);
		glActiveTexture(GL_TEXTURE0 + texture_unit_);
		glBindTexture(GL_TEXTURE_2D, id_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);
		if (int width, height, channels; auto* data = stbi_load(filepath.data(), &width, &height, &channels, 0)) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		} else {
			std::ostringstream oss;
			oss << "Failed to open " << filepath;
			throw std::invalid_argument{oss.str()};
		}
	}

	Texture2d(const Texture2d&) = delete;
	Texture2d(Texture2d&&) noexcept = delete;
	Texture2d& operator=(const Texture2d&) = delete;
	Texture2d& operator=(Texture2d&&) noexcept = delete;

	~Texture2d() {
		glDeleteTextures(1, &id_);
	}

	void Bind() const noexcept {
		glActiveTexture(GL_TEXTURE0 + texture_unit_);
		glBindTexture(GL_TEXTURE_2D, id_);
	}

private:
	inline static GLuint instance_count_{0};
	GLuint id_{0};
	GLuint texture_unit_{0};
};