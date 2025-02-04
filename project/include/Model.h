#pragma once
#include "tiny_obj_loader.h"
#include <exception>

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

    Model(Shader& shader, Texture& texture, std::string model_path);
    
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

    void createBuffer();
    void loadOBJ(const std::string& path);
};