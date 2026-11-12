#pragma once

#include <string>
#include <regex>

enum TokenType {
    WhiteSpace,
    LineBreak,
    ObjectType,
    Identifier,
    FileEnd,
};

struct Token {
    TokenType type;
    std::string value;
    int lineNumber;
    int columnNumber;
    int pos;
};

struct TokenRule {
    std::regex matcher;
    TokenType type;
};
