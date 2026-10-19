#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>

#include "Renderer.h"

Renderer::Renderer() : screenShader("shaders/screen.vert", "shaders/screen.frag") {
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
    screenShader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*


    glDeleteShader(screenVert);
    glDeleteShader(screenFrag);
*/
