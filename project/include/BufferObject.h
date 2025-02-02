#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class BufferObject {
private:
	GLuint m_bufferID;
public:
	BufferObject(GLuint idx, GLint size, GLenum dtype) {
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glVertexAttribPointer(idx, size, dtype, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(idx);
	}

	void load(GLsizeiptr size, const void* data, GLenum usage) {
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	}
};
