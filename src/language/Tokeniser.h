#pragma once

#include <string>
#include <vector>

#include "Token.h"

class Tokeniser {
public:
    Tokeniser(std::string& source);
    
    void tokenise();
    
    std::vector<Token> getTokens();
    
private:
    std::string source;
    std::vector<Token> tokens;
    
    std::vector<TokenRule> tokenRules;
};
