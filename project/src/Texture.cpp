#include "Texture.h"
#include "stb_image.h"
#include <exception>


Texture::Texture(std::string path) {
	glGenTextures(1, &m_id);

	int components;

	unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &components, 0);

	if (data != nullptr) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		bind();

		//Set number of components by format of the texture
		GLuint format = GL_RGB;

		switch (components) {
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 2:
			format = GL_RG;
			break;
		case 1:
			format = GL_RED;
			break;
		default:
			throw std::exception("Invalid Texture Format");
			break;
		};

		//Loads texture data into bound texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clean up
		unbind();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//Return to default alignment
		stbi_image_free(data);

	}
	else {
		throw std::exception("Failed to read texture data from file!");
	}
}

Texture::~Texture() { glDeleteTextures(1, &m_id); }

int Texture::getWidth() const{ return m_width; }
int Texture::getHeight() const{ return m_height; }

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, m_id); }
void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
