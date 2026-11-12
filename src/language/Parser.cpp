#include "Parser.h"

#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(tokens) {
    
}

void Parser::parse() {
    const std::vector<std::string> typeKeywords = {"SQUARE", "CUBE"};
    const std::vector<std::string> actionKeywords = {"fadeIn", "fadeOut", "draw", "show", "edit"};
    
    pos = 0;
}

Token Parser::peek() {
    return tokens[pos];
}

Token Parser::consume(std::string expected) {
    if (tokens[pos].value != expected) {
        std::cerr << "Expected " << expected << " got " << tokens[pos].value << std::endl;
    }
    return tokens[pos++];
}

Token Parser::consumeType(TokenType expected) {
    if (tokens[pos].type != expected) {
        // std::cerr << "Expected " << expected << " type got " << tokens[pos].type << " type" << std::endl;
    }
    return tokens[pos++];
}


