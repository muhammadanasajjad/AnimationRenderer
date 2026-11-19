#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Renderer.h"
#include "../scene/Material.h"
#include "../scene/Triangle.h"

Renderer::Renderer() :
    projectionShader("shaders/projection.comp"),
    countShader("shaders/count.glsl"),
    prefixShader("shaders/prefix.glsl"),
    fillShader("shaders/fill.glsl"),
    screenShader("shaders/screen.vert", "shaders/screen.frag")
{
    projectionShader.use();

    projectionShader.setVec3("cameraPosition", glm::vec3(-15.0f, 0.0f, 0.0f));
    projectionShader.setVec3("cameraForward", glm::vec3(1.0f, 0.0f, 0.0f));
    projectionShader.setVec3("cameraUp", glm::vec3(0.0f, 1.0f, 0.0f));
    projectionShader.setVec3("cameraRight", glm::cross(
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    ));
    projectionShader.setUInt("triangleCount", 0);

    glGenBuffers(1, &triangleSSBO);
    glGenBuffers(1, &materialSSBO);
    glGenBuffers(1, &tileCountSSBO);
    glGenBuffers(1, &tileOffsetSSBO);
    glGenBuffers(1, &tileCounterSSBO);
    glGenBuffers(1, &tileTriangleSSBO);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Renderer::ensureBufferSize(unsigned int& buffer, std::size_t& capacity, std::size_t bytes) {
    std::size_t allocation = std::max<std::size_t>(bytes, 4);

    if (allocation <= capacity) {
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, allocation, nullptr, GL_DYNAMIC_DRAW);
    capacity = allocation;
}

void Renderer::render(std::vector<Triangle>& triangles, std::vector<Material>& materials) {
    const int triangleCount = static_cast<int>(triangles.size());
    const int tileCount = TileRows * TileColumns;
    const std::size_t triangleBytes = triangles.size() * sizeof(Triangle);
    const std::size_t materialBytes = materials.size() * sizeof(Material);
    const std::size_t tileIntBytes = static_cast<std::size_t>(tileCount) * sizeof(int);
    const std::size_t tileOffsetBytes = (static_cast<std::size_t>(tileCount) + 1) * sizeof(int);
    const std::size_t tileTriangleBytes = std::max<std::size_t>(
        4,
        static_cast<std::size_t>(triangleCount) * static_cast<std::size_t>(tileCount) * sizeof(int)
    );

    ensureBufferSize(triangleSSBO, triangleCapacity, triangleBytes);
    ensureBufferSize(materialSSBO, materialCapacity, materialBytes);
    ensureBufferSize(tileCountSSBO, tileCountCapacity, tileIntBytes);
    ensureBufferSize(tileOffsetSSBO, tileOffsetCapacity, tileOffsetBytes);
    ensureBufferSize(tileCounterSSBO, tileCounterCapacity, tileIntBytes);
    ensureBufferSize(tileTriangleSSBO, tileTriangleCapacity, tileTriangleBytes);

    if (triangleBytes > 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, triangleBytes, triangles.data());
    }

    if (materialBytes > 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, materialBytes, materials.data());
    }

    std::vector<int> zeros(static_cast<std::size_t>(tileCount), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tileCountSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, zeros.size() * sizeof(int), zeros.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tileCounterSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, zeros.size() * sizeof(int), zeros.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tileCountSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tileOffsetSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, tileCounterSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, tileTriangleSSBO);

    projectionShader.use();
    projectionShader.setUInt("triangleCount", triangleCount);

    if (triangleCount > 0) {
        glDispatchCompute((triangleCount + 255) / 256, 1, 1);
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    if (triangleCount > 0) {
        countShader.use();
        countShader.setInt("projectedTriangleCount", triangleCount);
        countShader.setInt("tileRows", TileRows);
        countShader.setInt("tileColumns", TileColumns);
        glDispatchCompute((triangleCount + 63) / 64, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    prefixShader.use();
    prefixShader.setInt("tileCount", tileCount);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    if (triangleCount > 0) {
        fillShader.use();
        fillShader.setInt("projectedTriangleCount", triangleCount);
        fillShader.setInt("tileRows", TileRows);
        fillShader.setInt("tileColumns", TileColumns);
        fillShader.setInt("tileTriangleCapacity", static_cast<int>(tileTriangleCapacity / sizeof(int)));
        glDispatchCompute((triangleCount + 63) / 64, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    screenShader.use();
    screenShader.setInt("tileRows", TileRows);
    screenShader.setInt("tileColumns", TileColumns);
    screenShader.setInt("tileTriangleCapacity", static_cast<int>(tileTriangleCapacity / sizeof(int)));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
