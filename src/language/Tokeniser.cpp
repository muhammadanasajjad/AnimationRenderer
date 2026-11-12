#include "Tokeniser.h"

#include <iostream>

Tokeniser::Tokeniser(std::string& source) : source(source) {
    tokenRules = {
        { std::regex("\\n+"), TokenType::LineBreak },
        { std::regex(" +"), TokenType::WhiteSpace },
        { std::regex("[A-Z_]+"), TokenType::ObjectType },
        { std::regex("[a-zA-Z_0-9]+"), TokenType::Identifier }
    };
}

void Tokeniser::tokenise() {
    // meaning tokens is already filled
    if (tokens.size() != 0) return;
    
    int pos = 0;
    int lineNumber = 1;
    int columnNumber = 1;
    
    while (pos < source.length()) {
        bool matched = false;
        
        for (const TokenRule& rule : tokenRules) {
            std::smatch matchInfo;
            std::string remaining = source.substr(pos);
            const bool match = std::regex_search(
                remaining,
                matchInfo,
                rule.matcher,
                std::regex_constants::match_continuous
            );
            
            if (!match) continue;
            matched = true;
            
            std::string matchingSubstring = matchInfo.str();
            tokens.push_back(Token(
                rule.type, 
                matchingSubstring, 
                lineNumber,
                columnNumber,
                pos
            ));
            pos += matchInfo.length();
            
            Token token = tokens[tokens.size() - 1];
            const char* typeName = (token.type == ObjectType) ? "objectType" : (token.type == WhiteSpace) ? "whitespace" : "identifier";
            std::cout << "Token{"
                << "type=" << typeName
                << ", value=\"" << token.value << "\""
                << ", lineNumber=" << token.lineNumber
                << ", columnNumber=" << token.columnNumber
                << ", pos=" << token.pos
                << "}" << std::endl;
            
            if (token.type != LineBreak) columnNumber += matchInfo.length();
            else {
                columnNumber = 1;
                lineNumber += matchInfo.length();
            }
            break;
        }
        if (!matched) { 
            // throw error token is invalid
        }
    }
    
    tokens.push_back(Token(FileEnd, "", lineNumber, columnNumber, pos));
}

std::vector<Token> Tokeniser::getTokens() {
    return tokens;
}
