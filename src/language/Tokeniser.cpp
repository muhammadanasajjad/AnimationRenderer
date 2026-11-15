#include "Tokeniser.h"

#include <iostream>

Tokeniser::Tokeniser(std::string& source) : source(source) {
    tokenRules = {
        { std::regex("\\n+"), TokenType::LineBreak },
        { std::regex(" +"), TokenType::WhiteSpace },
        
        { std::regex(":"), TokenType::Colon },
        { std::regex("\\{"), TokenType::OpenCurlyBrace },
        { std::regex("\\}"), TokenType::CloseCurlyBrace },
        { std::regex("\\("), TokenType::OpenParenthesis },
        { std::regex("\\)"), TokenType::CloseParenthesis },
        { std::regex(","), TokenType::Comma },
        { std::regex("="), TokenType::Equals },
        { std::regex("\\^"), TokenType::Caret },
        { std::regex("\\*"), TokenType::Asterisk },
        { std::regex("/"), TokenType::ForwardSlash },
        { std::regex("\\+"), TokenType::Plus },
        { std::regex("-"), TokenType::Minus },
        
        { std::regex("[0-9]+"), TokenType::IntegerLiteral },
        { std::regex("[0-9]+\\.[0-9]+"), TokenType::FloatLiteral },
        { 
            std::regex("\"[^\"\\n]*\""), // start with " any character except quotation and new line 0+ then end with "
            TokenType::StringLiteral
        },
        { std::regex("[A-Z_]+"), TokenType::ObjectType },
        { std::regex("[a-zA-Z_0-9]+"), TokenType::Identifier }
    };
}



void debugToken(const Token& token) {
    const char* typeNames[] = {
        "LineBreak",
        "WhiteSpace",
        "Colon",
        "OpenCurlyBrace",
        "CloseCurlyBrace",
        "OpenParenthesis",
        "CloseParenthesis",
        "Comma",
        "Equals",
        "Caret",
        "Asterisk",
        "ForwardSlash",
        "Plus",
        "Minus",
        "IntegerLiteral",
        "FloatLiteral",
        "StringLiteral",
        "ObjectType",
        "Identifier",
        "FileEnd"
    };

    std::cout
        << "Token { "
        << "type: " << typeNames[token.type]
        << ", value: \"" << token.value << "\""
        // << ", line: " << token.lineNumber
        // << ", column: " << token.columnNumber
        // << ", pos: " << token.pos
        << " }"
        << std::endl;
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

            if (rule.type == TokenType::StringLiteral)
                matchingSubstring = matchingSubstring.substr(1, matchingSubstring.length() - 2);

            tokens.push_back(Token(
                rule.type, 
                matchingSubstring, 
                lineNumber,
                columnNumber,
                pos
            ));
            pos += matchInfo.length();
            
            Token token = tokens[tokens.size() - 1];
            debugToken(token);
            
            if (token.type != LineBreak) columnNumber += matchInfo.length();
            else {
                columnNumber = 1;
                lineNumber += matchInfo.length();
            }
            
            if (token.type == WhiteSpace) {
                tokens.pop_back();
            }
            
            break;
        }
        if (!matched) {
            // TODO: add actual error handling
            std::cerr << "token didn't match" << std::endl;
            break;
        }
    }
    
    tokens.push_back(Token(FileEnd, "", lineNumber, columnNumber, pos));
}

std::vector<Token> Tokeniser::getTokens() {
    return tokens;
}
