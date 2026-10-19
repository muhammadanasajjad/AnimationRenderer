#pragma once

#include <string>

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);

    void use();
    
private:
    unsigned int programId;
};

