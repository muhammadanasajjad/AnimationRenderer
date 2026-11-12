#include "Parser.h"

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
        return; // throw error
    }
    return tokens[pos++];
}

Token Parser::consumeType(TokenType expected) {
    if (tokens[pos].type != expected) {
        return; // throw error
    }
    return tokens[pos++];
}
