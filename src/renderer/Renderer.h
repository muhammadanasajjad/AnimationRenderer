#pragma once

#include <vector>

#include "Shader.h"
#include "../scene/Triangle.h"
#include "../scene/Material.h"

class Renderer {
public:
    Renderer();
    void render(std::vector<Triangle>& triangles, std::vector<Material>& materials);
private:
    Shader projectionShader;
    Shader screenShader;
    unsigned int triangleSSBO;
    unsigned int materialSSBO;
    
    unsigned int VAO;
    unsigned int VBO;
};
