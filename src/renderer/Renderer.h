#pragma once

#include <cstddef>
#include <vector>

#include "Shader.h"
#include "../scene/Triangle.h"
#include "../scene/Material.h"

class Renderer {
public:
    Renderer();
    void render(std::vector<Triangle>& triangles, std::vector<Material>& materials);
private:
    static constexpr int TileRows = 8;
    static constexpr int TileColumns = 8;

    void ensureBufferSize(unsigned int& buffer, std::size_t& capacity, std::size_t bytes);

    Shader projectionShader;
    Shader countShader;
    Shader prefixShader;
    Shader fillShader;
    Shader screenShader;

    unsigned int triangleSSBO;
    unsigned int materialSSBO;
    unsigned int tileCountSSBO;
    unsigned int tileOffsetSSBO;
    unsigned int tileCounterSSBO;
    unsigned int tileTriangleSSBO;

    std::size_t triangleCapacity = 0;
    std::size_t materialCapacity = 0;
    std::size_t tileCountCapacity = 0;
    std::size_t tileOffsetCapacity = 0;
    std::size_t tileCounterCapacity = 0;
    std::size_t tileTriangleCapacity = 0;

    unsigned int VAO;
    unsigned int VBO;
};
