#pragma once

#include <glad/glad.h>
#include <string>
#include "Shader.h"

class BackgroundRenderer {
public:
    BackgroundRenderer(const std::string& texturePath, Shader* shader);
    ~BackgroundRenderer();

    void draw();

private:
    unsigned int backgroundTexture;
    Shader* shader;

    void loadTexture(const std::string& path);
};
