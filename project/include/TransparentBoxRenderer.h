#pragma once
#define TRANSPARENT_BOX_RENDERER_H

#include "Shader.h"

class TransparentBoxRenderer {
public:
    TransparentBoxRenderer();
    ~TransparentBoxRenderer();

    void draw(float x, float y, float width, float height, float alpha);

private:
    Shader* boxShader;
};
