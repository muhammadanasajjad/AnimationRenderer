#pragma once

#include "Shader.h"

class Renderer {
public:
    Renderer();
    void render();
private:
    Shader projectionShader;
    Shader screenShader;
    unsigned int triangleSSBO;
    
    unsigned int VAO;
    unsigned int VBO;
};
