#pragma once

#include <vector>
#include <iostream>
#include "Shader.h"
#include "Texture.h"

class Model {
public:
	Model(
        Shader& shader,
        Texture& texture,
        std::vector<float> vertices,
        std::vector<float> normals,
        std::vector<float> textCoords
    ) :
        m_shader(shader),
        m_texture(texture),
        m_vertices(vertices),
        m_normals(normals),
        m_textCoords(textCoords)
	{
        // Calculate the number of vertices: each vertex position is 3 floats.
        m_count = int(m_vertices.size() / 3);

        std::cout << vertices.size() << std::endl;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(3, VBO);

        // specify vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        // specify normal attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), &m_normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        // specify texture coordinate attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, m_textCoords.size() * sizeof(float), &m_textCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

    void draw() {
        m_shader.use();
        m_texture.bind();

        glBindVertexArray(VAO);

        // Draw the object as triangles
        glDrawArrays(GL_TRIANGLES, 0, m_count);

        // Unbind the VAO when done (optional)
        glBindVertexArray(0);
    }
private:
    Shader& m_shader;
    Texture& m_texture;
    int m_count;
	std::vector<float> m_vertices;
	std::vector<float> m_normals;
	std::vector<float> m_textCoords;

	GLuint VAO;
	GLuint VBO[3];
};