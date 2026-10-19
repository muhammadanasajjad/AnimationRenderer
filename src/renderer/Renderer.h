#pragma once

#include "Shader.h"

class Renderer {
public:
    Renderer();
    void render();
private:
    Shader screenShader;
    
    unsigned int VAO;
    unsigned int VBO;
};
