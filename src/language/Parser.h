#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ASTNode.h"
#include "Token.h"

struct OperatorInfo {
    int leftBinding;
    int rightBinding;
};

class Parser {
public:
    Parser(std::vector<Token> tokens);
    
    std::unique_ptr<ProgramNode> parse();
    
private:
    int pos = 0;
    
    std::unordered_map<std::string, OperatorInfo> expressionOperators;
    
    std::vector<Token> tokens;
    
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseTopLevelStatement();

    std::unique_ptr<ExpressionNode> parseExpression(int minBinding = 0);
    std::unique_ptr<ExpressionNode> parseUnaryExpression();
    std::unique_ptr<ExpressionNode> parsePrimaryExpression();

    std::unique_ptr<IntegerLiteralNode> parseIntegerLiteral();
    std::unique_ptr<FloatLiteralNode> parseFloatLiteral();
    std::unique_ptr<StringLiteralNode> parseStringLiteral();
    std::unique_ptr<IdentifierNode> parseIdentifierExpression();
    std::unique_ptr<ObjectExpressionNode> parseObjectExpression();
    std::unique_ptr<TupleExpressionNode> parseTupleExpression();

    std::unique_ptr<PreloadDeclarationNode> parsePreloadDeclaration();
    std::unique_ptr<SettingsDeclarationNode> parseSettingsDeclaration();
    std::unique_ptr<SceneDeclarationNode> parseSceneDeclaration();
    std::unique_ptr<InterpolationDeclarationNode> parseInterpolationDeclaration();
    std::unique_ptr<ObjectDeclarationNode> parseObjectDeclaration();

    std::unique_ptr<ASTNode> parseSceneStatement();
    std::unique_ptr<ASTNode> parseAction();
    std::unique_ptr<PlayActionNode> parsePlayAction();
    std::unique_ptr<FadeActionNode> parseFadeAction();
    std::unique_ptr<RotateActionNode> parseRotateAction();
    std::unique_ptr<MoveActionNode> parseMoveAction();
    std::unique_ptr<ScaleActionNode> parseScaleAction();
    std::unique_ptr<ShowActionNode> parseShowAction();
    std::unique_ptr<EditActionNode> parseEditAction();
    std::unique_ptr<WaitActionNode> parseWaitAction();

    bool isObjectExpressionStart();
    bool isStatementEnd();
    void skipSeparators();
    
    Token peek();
    Token peek(int distance);
    Token consume();
    Token consume(std::string expected);
    Token consume(TokenType expectedType);
    
};
