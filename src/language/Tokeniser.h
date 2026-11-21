#pragma once

#include <string>
#include <vector>

#include "Token.h"

class Tokeniser {
public:
    Tokeniser(const std::string& source);
    
    void tokenise(bool printDebug = true);
    
    std::vector<Token> getTokens();
    
private:
    std::string source;
    std::vector<Token> tokens;
    
    std::vector<TokenRule> tokenRules;
};
