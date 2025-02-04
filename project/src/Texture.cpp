#include "Texture.h"
#include "stb_image.h"
#include <exception>


Texture::Texture(std::string path, bool isJPG) {
	glGenTextures(1, &m_id);

	glBindTexture(GL_TEXTURE_2D, m_id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &nrChannels, 0);
	if (data) // if we got data, we loaded succesfully
	{
		if (isJPG) { // JPG needs RGBA
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else { // Likely PNG here
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else
	{
		throw std::exception("Failed to load texture AHF");
	}
	stbi_image_free(data); // Free
}

Texture::~Texture() { glDeleteTextures(1, &m_id); }

int Texture::getWidth() const{ return m_width; }
int Texture::getHeight() const{ return m_height; }

void Texture::bind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
