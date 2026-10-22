#pragma once

#include <vector>

#include "Shader.h"
#include "../scene/Triangle.h"

class Renderer {
public:
    Renderer();
    void render(std::vector<Triangle>& triangles);
private:
    Shader projectionShader;
    Shader screenShader;
    unsigned int triangleSSBO;
    
    unsigned int VAO;
    unsigned int VBO;
};
