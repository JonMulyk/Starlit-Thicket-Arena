#pragma once


#include "tiny_obj_loader.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "Shader.h"
#include "Texture.h"
#include <Mesh.h>

class Model {
public:
    // Constructor for manually provided data
    Model(
        Shader& shader,
        Texture& texture,
        std::vector<float> vertices,
        std::vector<float> normals,
        std::vector<float> textCoords
    );

    // Constructor for loading an OBJ file with a texture
    Model(Shader& shader, Texture& texture, const std::string& model_path);

    // Constructor for loading an OBJ file without a texture
    Model(Shader& shader, const std::string& model_path);


    void Draw(Shader& shader);

private:
    Shader& m_shader;
    Texture* m_texture; // Changed to a pointer to allow null values
    int m_count;
    bool hasTexture;
    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_textCoords;
    std::vector<tinyobj::material_t> m_materials;

    GLuint VAO;
    GLuint VBO[3];


    void createBuffer();
    void loadOBJ(const std::string& path);

};
