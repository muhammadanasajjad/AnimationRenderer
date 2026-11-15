#pragma once

#include <iosfwd>

#include "ASTNode.h"

class ASTPrinter {
public:
    static void print(const ASTNode* root);
    static void print(const ASTNode* root, std::ostream& out, bool useColor = true);
};
