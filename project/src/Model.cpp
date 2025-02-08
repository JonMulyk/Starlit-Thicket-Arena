#include "Model.h"

void Model::loadOBJ(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string basePath = "project/assets/models/";

    
    // Load the OBJ file
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), basePath.c_str());
    if (!ret) {
        throw std::runtime_error("Failed to load obj file: " + warn + err);
    }

    //edbug warnings
    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    //debug errors
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    //debug material
    if (materials.empty()) {
        std::cerr << "Error: No materials found." << std::endl;
    }

    // Loop through all shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;

        // Loop through all faces (polygons) in each shape
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop through all vertices in the face
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // access vertex data
                if (idx.vertex_index >= 0) {
                    m_vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
                    m_vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
                    m_vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
                }

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    m_normals.push_back(attrib.normals[3 * size_t(idx.normal_index) + 0]);
                    m_normals.push_back(attrib.normals[3 * size_t(idx.normal_index) + 1]);
                    m_normals.push_back(attrib.normals[3 * size_t(idx.normal_index) + 2]);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    m_textCoords.push_back(attrib.texcoords[2 * size_t(idx.texcoord_index) + 0]);
                    m_textCoords.push_back(attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]);
                }
            }
            index_offset += fv;

            shapes[s].mesh.material_ids[f];
        }
    }
}




void Model::createBuffer() {
    m_count = static_cast<int>(m_vertices.size() / 3);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(3, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), &m_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    if (!m_textCoords.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, m_textCoords.size() * sizeof(float), &m_textCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//manually provided data
Model::Model(
    Shader& shader,
    Texture& texture,
    std::vector<float> vertices,
    std::vector<float> normals,
    std::vector<float> textCoords
) : m_shader(shader), m_texture(&texture), m_vertices(vertices), m_normals(normals), m_textCoords(textCoords), hasTexture(true) {
    createBuffer();
}

//OBJ file with a texture
Model::Model(Shader& shader, Texture& texture, const std::string& model_path)
    : m_shader(shader), m_texture(&texture), hasTexture(true) {
    loadOBJ(model_path);
    createBuffer();
}

 //without a texture
Model::Model(Shader& shader, const std::string& model_path)
    : m_shader(shader), m_texture(nullptr), hasTexture(false) {
    loadOBJ(model_path);
    createBuffer();
}

void Model::Draw(Shader& shader) {
    m_shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_count);
    glBindVertexArray(0);
    
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
    if (hasTexture && m_texture) {
        m_texture->bind();
    }



}

