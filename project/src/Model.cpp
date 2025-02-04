#include "Model.h"

void Model::loadOBJ(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    //check if code does not load object, generally comes from name being wrong or path
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        std::string message = "Failed to load obj file: " + warn + err;
        throw std::exception(message.c_str());
    }

    for (const auto& shape : shapes) // loop over shapes
    {

        //DO LATER LOOP OVER FACES FOR MESHES
        for (const auto& index : shape.mesh.indices)
        {
            //currently  being scaled fo vertex
            if (index.vertex_index >= 0 && index.vertex_index < attrib.vertices.size() / 3)
            {
                m_vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                m_vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                m_vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            }


            if (index.normal_index >= 0 && index.normal_index < attrib.normals.size() / 3)
            {
                m_normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                m_normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                m_normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }


            if (index.texcoord_index >= 0 && index.texcoord_index < attrib.texcoords.size() / 2)
            {
                m_textCoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                m_textCoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }
        }
    }
}

void Model::createBuffer() {
    // Calculate the number of vertices: each vertex position is 3 floats.
    m_count = int(m_vertices.size() / 3);

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

Model::Model(
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
    createBuffer();
}

Model::Model(Shader& shader, Texture& texture, std::string model_path): m_shader(shader), m_texture(texture) {
    loadOBJ(model_path);
    createBuffer();
}

void Model::draw() {
    m_shader.use();
    m_texture.bind();

    glBindVertexArray(VAO);

    // Draw the object as triangles
    glDrawArrays(GL_TRIANGLES, 0, m_count);

    // Unbind the VAO when done (optional)
    glBindVertexArray(0);
}