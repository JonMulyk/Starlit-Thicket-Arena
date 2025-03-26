#pragma once
#include <string>
#include <glm.hpp>
#include <vector>
#include <Shader.h>
#include <assimp/scene.h>      
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h> 

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct TextureOBJ {
    unsigned int id;
    std::string type;
    aiString path;
};

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureOBJ>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureOBJ> textures);
    void Draw(Shader& shader, std::string entityName = "");
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

