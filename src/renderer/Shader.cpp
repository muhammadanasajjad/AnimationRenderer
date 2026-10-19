#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Shader.h"

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    std::ifstream vertFile(vertPath);
    if (!vertFile) {
        throw std::runtime_error("Failed to open " + vertPath);
    }
    std::stringstream vertBuffer;
    vertBuffer << vertFile.rdbuf();
    std::string vertStringSource = vertBuffer.str();
    
    std::ifstream fragFile(fragPath);
    if (!fragFile) {
        throw std::runtime_error("Failed to open " + fragPath);
    }
    std::stringstream fragBuffer;
    fragBuffer << fragFile.rdbuf();
    std::string fragStringSource = fragBuffer.str();
    
    const char* vertSource = vertStringSource.c_str();
    unsigned int vertId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertId, 1, &vertSource, NULL);
    glCompileShader(vertId);
    
    int success;
    glGetShaderiv(vertId, GL_COMPILE_STATUS, &success);

    if (!success) {
        int maxLength = 0;
        glGetShaderiv(vertId, GL_INFO_LOG_LENGTH, &maxLength);

        std::string errorLog;
        errorLog.resize(maxLength);
        glGetShaderInfoLog(vertId, maxLength, &maxLength, &errorLog[0]);
        
        glDeleteShader(vertId);
        throw std::runtime_error("ERROR::SHADER::VERTEX::COMPILATION_FAILED " + vertPath + "\n" + errorLog);
    }
    
    const char* fragSource = fragStringSource.c_str();
    unsigned int fragId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragId, 1, &fragSource, NULL);
    glCompileShader(fragId);
    
    glGetShaderiv(fragId, GL_COMPILE_STATUS, &success);

    if (!success) {
        int maxLength = 0;
        glGetShaderiv(fragId, GL_INFO_LOG_LENGTH, &maxLength);

        std::string errorLog;
        errorLog.resize(maxLength);
        glGetShaderInfoLog(fragId, maxLength, &maxLength, &errorLog[0]);

        glDeleteShader(fragId);
        throw std::runtime_error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " + fragPath + "\n" + errorLog);
    }
    
    programId = glCreateProgram();
    
    glAttachShader(programId, vertId);
    glAttachShader(programId, fragId);
    glLinkProgram(programId);
    
    glDeleteShader(vertId);
    glDeleteShader(fragId);
}

void Shader::use() {
    glUseProgram(programId);
}
