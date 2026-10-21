#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Renderer.h"

struct Triangle {
    glm::vec4 a;
    glm::vec4 b;
    glm::vec4 c;
};

Renderer::Renderer() : 
    projectionShader("shaders/projection.comp"),
    screenShader("shaders/screen.vert", "shaders/screen.frag")
{
    std::vector<Triangle> triangles = {
        // bottom (y = -2)
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f) },
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f) },

        // top (y = +2)
        { glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },
        { glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },

        // side A-B
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f) },
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f) },

        // side B-C
        { glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },
        { glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f) },

        // side C-D
        { glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },
        { glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },

        // side D-A
        { glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f) },
        { glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },
    };
    
    projectionShader.use();
    
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

void Renderer::render() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    screenShader.use();
    screenShader.setUInt("triangleCount", 24);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleSSBO);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*


    glDeleteShader(screenVert);
    glDeleteShader(screenFrag);
*/
