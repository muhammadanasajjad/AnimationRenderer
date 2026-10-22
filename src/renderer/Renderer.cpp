#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Renderer.h"
#include "../scene/Triangle.h"

Renderer::Renderer() : 
    projectionShader("shaders/projection.comp"),
    screenShader("shaders/screen.vert", "shaders/screen.frag")
{   
    projectionShader.use();
    
    std::vector<Triangle> triangles = {};
    
    projectionShader.setVec3("cameraPosition", glm::vec3(0.0f, 0.0f, 0.0f));
    projectionShader.setVec3("cameraForward", glm::vec3(1.0f, 0.0f, 0.0f));
    projectionShader.setVec3("cameraUp", glm::vec3(0.0f, 1.0f, 0.0f));
    projectionShader.setVec3("cameraRight", glm::cross(
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    ));
    projectionShader.setUInt("triangleCount", triangles.size());
    
    glGenBuffers(1, &triangleSSBO);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);
    // TODO: update the binding number later
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleSSBO);
    
    glDispatchCompute((triangles.size() + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    screenShader.use();
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


void Renderer::render(std::vector<Triangle>& triangles) {
    projectionShader.use();

    projectionShader.setUInt("triangleCount", triangles.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        triangles.size() * sizeof(Triangle),
        triangles.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleSSBO);

    if (!triangles.empty()) {
        glDispatchCompute((triangles.size() + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    screenShader.use();
    screenShader.setUInt("triangleCount", triangles.size());

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*


    glDeleteShader(screenVert);
    glDeleteShader(screenFrag);
*/
