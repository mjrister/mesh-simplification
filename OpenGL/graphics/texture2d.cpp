#include "graphics/texture2d.h"

#include <sstream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

	/** \brief Gets the maximum number of texture units allowed by the host GPU. */
	GLint GetMaxTextureImageUnits() {
		static GLint max_texture_image_units = 0;
		if (!max_texture_image_units) {
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
		}
		return max_texture_image_units;
	}
}

gfx::Texture2d::Texture2d(const std::string_view filepath, const std::uint8_t texture_unit_index)
	: texture_unit_index_{texture_unit_index} {

	if (texture_unit_index >= GetMaxTextureImageUnits()) throw std::out_of_range{"Texture unit index out of range"};

	glActiveTexture(GL_TEXTURE0 + texture_unit_index_);
	glGenTextures(1, &id_);
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
		throw std::runtime_error{oss.str()};
	}
}
