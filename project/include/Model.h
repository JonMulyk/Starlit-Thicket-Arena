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
    );
    
    void draw();

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