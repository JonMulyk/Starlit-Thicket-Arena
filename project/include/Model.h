#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm.hpp>

#include "BufferObject.h"

class Model {
private:
	GLuint m_vaoID;

	BufferObject vert_buff;
	BufferObject color_buff;
	BufferObject texture_buff;

public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec2> textures;

	Model() : vert_buff(0, 3, GL_FLOAT), color_buff(1, 3, GL_FLOAT), texture_buff(2, 2, GL_FLOAT) {
		// initialize vertex array object
		glGenVertexArrays(1, &m_vaoID);
		glBindVertexArray(m_vaoID);
	}

	void bind() {
		glBindVertexArray(m_vaoID);
	}

	void load() {
		bind();
		vert_buff.load(sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		color_buff.load(sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
		texture_buff.load(sizeof(glm::vec3) * textures.size(), textures.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}

	void draw() {
		glDrawArrays(GL_POINTS, 0, GLsizei(vertices.size()));
	}

};