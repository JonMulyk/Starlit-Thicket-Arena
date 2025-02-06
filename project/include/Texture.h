#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm.hpp>

class Texture {
private:
	GLuint m_id;
	int m_width;
	int m_height;
public:
	Texture(std::string path, bool isJPG);
	~Texture();

	int getWidth() const;
	int getHeight() const;

	void bind();
	void unbind();
};