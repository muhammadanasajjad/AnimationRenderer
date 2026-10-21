#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    Shader(const std::string& compPath);
    
    void setUInt(const std::string& name, unsigned int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);

    void use();
    
private:
    unsigned int programId;
};

