#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {
private:
    GLint m_id;
public:
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

    void checkCompileErrors(unsigned int shader, std::string type);

    operator GLint() const;
};

