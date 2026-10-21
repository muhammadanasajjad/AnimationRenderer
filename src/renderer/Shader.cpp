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
    
    int linked;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if (!linked) {
        int len = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(programId, len, &len, &log[0]);
        throw std::runtime_error("PROGRAM LINK FAILED\n" + log);
    }
    
    glDeleteShader(vertId);
    glDeleteShader(fragId);
}

Shader::Shader(const std::string& compPath) {
    std::ifstream compFile(compPath);
    if (!compFile) {
        throw std::runtime_error("Failed to open " + compPath);
    }
    std::stringstream compBuffer;
    compBuffer << compFile.rdbuf();
    std::string compStringSource = compBuffer.str();
    
    const char* compSource = compStringSource.c_str();
    unsigned int compId = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compId, 1, &compSource, NULL);
    glCompileShader(compId);
    
    int success;
    glGetShaderiv(compId, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        int maxLength = 0;
        glGetShaderiv(compId, GL_INFO_LOG_LENGTH, &maxLength);

        std::string errorLog;
        errorLog.resize(maxLength);
        glGetShaderInfoLog(compId, maxLength, &maxLength, &errorLog[0]);
        
        glDeleteShader(compId);
        throw std::runtime_error("ERROR::SHADER::COMPUTE::COMPILATION_FAILED " + compPath + "\n" + errorLog);
    }
    
    programId = glCreateProgram();
    
    glAttachShader(programId, compId);
    glLinkProgram(programId);
    
    int linked;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if (!linked) {
        int len = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(programId, len, &len, &log[0]);
        throw std::runtime_error("PROGRAM LINK FAILED\n" + log);
    }
    
    glDeleteShader(compId);
}

void Shader::setUInt(const std::string& name, unsigned int value) {
    glUniform1ui(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::use() {
    glUseProgram(programId);
}
