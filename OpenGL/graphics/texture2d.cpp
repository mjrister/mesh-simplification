#include "texture2d.h"

#include <sstream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

gfx::Texture2d::Texture2d(const std::string_view filepath, const GLenum texture_unit) : texture_unit_{texture_unit} {

	glGenTextures(1, &id_);
	glActiveTexture(texture_unit);
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

gfx::Texture2d::~Texture2d() {
	if (id_) {
		glDeleteTextures(1, &id_);
	}
}

void gfx::Texture2d::Bind() const noexcept {
	glActiveTexture(texture_unit_);
	glBindTexture(GL_TEXTURE_2D, id_);
}
