#pragma once

#include <string>
#include <regex>

enum TokenType {
    LineBreak,
    WhiteSpace,
    Colon,
    OpenCurlyBrace,
    CloseCurlyBrace,
    OpenParenthesis,
    CloseParenthesis,
    Comma,
    Equals,
    Caret,
    Asterisk,
    ForwardSlash,
    Plus,
    Minus,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
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
