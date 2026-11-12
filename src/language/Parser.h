#pragma once

#include "Token.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);
    
    void parse();
    
private:
    int pos = 0;
    
    std::vector<Token> tokens;
    
    Token peek();
    Token consume(std::string expected);
    Token consumeType(TokenType expectedType);
    
};
