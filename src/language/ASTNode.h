#pragma once

#include <memory>
#include <string>
#include <vector>

class ASTNode {
public:
    //idk yet
    virtual ~ASTNode() = default;
};

class ProgramNode {
public:
    std::vector<std::unique_ptr<ASTNode>> children;
};
