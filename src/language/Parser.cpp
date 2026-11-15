#include "Parser.h"

#include <iostream>
#include <unordered_set>

#include "ASTNode.h"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens) {
    expressionOperators = {
        {"+", {10, 11}},
        {"-", {10, 11}},
        {"*", {20, 21}},
        {"/", {20, 21}},
        {"^", {30, 30}}
    };
}

std::unique_ptr<ProgramNode> Parser::parse() {
    pos = 0;
    
    return parseProgram();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();

    while (pos < tokens.size()) {
        skipSeparators();

        if (pos >= tokens.size() || peek().type == TokenType::FileEnd)
            break;

        program->children.push_back(parseTopLevelStatement());
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseTopLevelStatement() {
    if (peek().value == "PRELOAD")
        return parsePreloadDeclaration();
    
    if (peek().value == "SETTINGS") 
        return parseSettingsDeclaration();

    if (peek().value == "SCENE")
        return parseSceneDeclaration();

    if (peek().value == "play")
        return parsePlayAction();
    
    throw std::runtime_error("Unexpected top-level token " + peek().value);
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(int minBinding) {
    auto left = parseUnaryExpression();

    static const std::unordered_set<std::string> units = {
        "fps", "ms", "s", "deg", "rad"
    };

    while (true) {
        if (peek().type == TokenType::LineBreak ||
            peek().type == TokenType::Comma ||
            peek().type == TokenType::FileEnd ||
            peek().value == "}" ||
            peek().value == ")") {
            break;
        }

        if (peek().type == TokenType::OpenParenthesis) {

            auto call = std::make_unique<FunctionCallExpressionNode>();
            call->callee = std::move(left);

            consume(TokenType::OpenParenthesis);

            while (peek().value != ")") {
                skipSeparators();

                if (peek().value == ")")
                    break;

                call->arguments.push_back(parseExpression());
            }

            consume(TokenType::CloseParenthesis);

            left = std::move(call);
            continue;
        }

        if (peek().type == TokenType::Identifier &&
            units.contains(peek().value)) {

            auto unit = std::make_unique<UnitExpressionNode>();
            unit->value = std::move(left);
            unit->unit = consume().value;

            left = std::move(unit);
            continue;
        }

        auto it = expressionOperators.find(peek().value);

        if (it == expressionOperators.end())
            break;

        OperatorInfo info = it->second;

        if (info.leftBinding <= minBinding)
            break;

        Token operation = consume();

        auto right = parseExpression(info.rightBinding);

        auto binary = std::make_unique<BinaryExpressionNode>();
        binary->operation = operation.value;
        binary->left = std::move(left);
        binary->right = std::move(right);

        left = std::move(binary);
    }

    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryExpression() {
    if (peek().type == TokenType::Minus ||
        peek().type == TokenType::Plus) {

        Token operation = consume();

        auto unary = std::make_unique<UnaryExpressionNode>();
        unary->operation = operation.value;
        unary->operand = parseUnaryExpression();

        return unary;
    }

    return parsePrimaryExpression();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
    if (peek().value == "{") {
        if (isObjectExpressionStart())
            return parseObjectExpression();

        return parseTupleExpression();
    }

    if (peek().type == TokenType::IntegerLiteral)
        return parseIntegerLiteral();

    if (peek().type == TokenType::FloatLiteral)
        return parseFloatLiteral();

    if (peek().type == TokenType::StringLiteral)
        return parseStringLiteral();

    if (peek().type == TokenType::Identifier)
        return parseIdentifierExpression();

    if (peek().type == TokenType::OpenParenthesis) {
        consume();

        auto expression = parseExpression();

        consume(TokenType::CloseParenthesis);

        return expression;
    }

    throw std::runtime_error("Unexpected token in primary expression: " + peek().value);
}

std::unique_ptr<IntegerLiteralNode> Parser::parseIntegerLiteral() {
    Token token = consume();

    auto literal = std::make_unique<IntegerLiteralNode>();
    literal->value = std::stod(token.value);
    literal->isInteger = true;

    return literal;
}

std::unique_ptr<FloatLiteralNode> Parser::parseFloatLiteral() {
    Token token = consume();

    auto literal = std::make_unique<FloatLiteralNode>();
    literal->value = std::stod(token.value);

    return literal;
}

std::unique_ptr<StringLiteralNode> Parser::parseStringLiteral() {
    Token token = consume();

    auto literal = std::make_unique<StringLiteralNode>();
    literal->value = token.value;

    return literal;
}

std::unique_ptr<IdentifierNode> Parser::parseIdentifierExpression() {
    Token token = consume();

    auto identifier = std::make_unique<IdentifierNode>();
    identifier->name = token.value;

    return identifier;
}

std::unique_ptr<ObjectExpressionNode> Parser::parseObjectExpression() {
    auto objectExpression = std::make_unique<ObjectExpressionNode>();

    consume("{");

    while (pos < tokens.size() && peek().value != "}") {
        while (pos < tokens.size() &&
               (peek().type == TokenType::Comma ||
                peek().type == TokenType::LineBreak)) {
            consume();
        }

        if (pos >= tokens.size())
            break;

        if (peek().value == "}")
            break;

        if (peek().type != TokenType::Identifier)
            throw std::runtime_error("Expected identifier, got: " + peek().value);

        Token parameterName = consume();

        consume(":");

        objectExpression->properties[parameterName.value] = parseExpression();
    }

    consume("}");

    return objectExpression;
}

std::unique_ptr<TupleExpressionNode> Parser::parseTupleExpression() {
    auto tuple = std::make_unique<TupleExpressionNode>();

    consume("{");

    skipSeparators();

    while (pos < tokens.size() && peek().value != "}") {
        tuple->elements.push_back(parseExpression());

        skipSeparators();
    }

    consume("}");

    return tuple;
}

bool Parser::isObjectExpressionStart() {
    if (peek().value != "{")
        return false;

    if (peek(1).value == "}")
        return true;

    return peek(1).type == TokenType::Identifier &&
           peek(2).value == ":";
}

bool Parser::isStatementEnd() {
    Token token = peek();

    return token.type == TokenType::LineBreak ||
           token.type == TokenType::Comma ||
           token.type == TokenType::FileEnd ||
           token.value == "}";
}

void Parser::skipSeparators() {
    while (pos < tokens.size() &&
           (peek().type == TokenType::LineBreak ||
            peek().type == TokenType::Comma)) {
        consume();
    }
}

std::unique_ptr<SceneDeclarationNode> Parser::parseSceneDeclaration() {
    auto scene = std::make_unique<SceneDeclarationNode>();

    consume("SCENE");

    scene->name = consume(TokenType::Identifier).value;

    consume(":");
    consume("{");

    skipSeparators();

    while (pos < tokens.size() && peek().value != "}") {
        scene->children.push_back(parseSceneStatement());

        skipSeparators();
    }

    consume("}");

    return scene;
}

std::unique_ptr<InterpolationDeclarationNode> Parser::parseInterpolationDeclaration() {
    auto interpolation = std::make_unique<InterpolationDeclarationNode>();

    consume("INTERPOLATION");

    interpolation->name = consume(TokenType::Identifier).value;

    consume(":");
    consume("{");

    skipSeparators();

    consume("function");
    consume(":");

    interpolation->functionName = consume(TokenType::Identifier).value;

    consume(TokenType::OpenParenthesis);
    interpolation->parameter = consume(TokenType::Identifier).value;
    consume(TokenType::CloseParenthesis);

    consume("=");

    interpolation->function = parseExpression();

    skipSeparators();

    consume("}");

    return interpolation;
}

std::unique_ptr<ObjectDeclarationNode> Parser::parseObjectDeclaration() {
    auto object = std::make_unique<ObjectDeclarationNode>();

    object->objectType = consume(TokenType::ObjectType).value;
    object->name = consume(TokenType::Identifier).value;

    consume(":");

    object->properties = parseObjectExpression();

    return object;
}

std::unique_ptr<ASTNode> Parser::parseSceneStatement() {
    static const std::unordered_set<std::string> actionKeywords = {
        "fadeIn", "fadeOut", "rotate", "move", "scale", "show", "edit", "wait"
    };

    if (peek().value == "INTERPOLATION")
        return parseInterpolationDeclaration();

    if (peek().type == TokenType::ObjectType)
        return parseObjectDeclaration();

    if (actionKeywords.contains(peek().value))
        return parseAction();

    throw std::runtime_error("Unexpected scene statement: " + peek().value);
}

std::unique_ptr<ASTNode> Parser::parseAction() {
    if (peek().value == "fadeIn" || peek().value == "fadeOut")
        return parseFadeAction();

    if (peek().value == "rotate")
        return parseRotateAction();

    if (peek().value == "move")
        return parseMoveAction();

    if (peek().value == "scale")
        return parseScaleAction();

    if (peek().value == "show")
        return parseShowAction();

    if (peek().value == "edit")
        return parseEditAction();

    if (peek().value == "wait")
        return parseWaitAction();

    throw std::runtime_error("Unexpected action: " + peek().value);
}

std::unique_ptr<PlayActionNode> Parser::parsePlayAction() {
    auto action = std::make_unique<PlayActionNode>();

    consume("play");

    action->scene = consume(TokenType::Identifier).value;

    return action;
}

std::unique_ptr<FadeActionNode> Parser::parseFadeAction() {
    auto action = std::make_unique<FadeActionNode>();

    Token keyword = consume();

    action->isFadeIn = keyword.value == "fadeIn";
    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<RotateActionNode> Parser::parseRotateAction() {
    auto action = std::make_unique<RotateActionNode>();

    consume("rotate");

    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "{")
        action->amount = parseObjectExpression();
    else if (!isStatementEnd())
        action->amount = parseExpression();

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<MoveActionNode> Parser::parseMoveAction() {
    auto action = std::make_unique<MoveActionNode>();

    consume("move");

    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "{")
        action->amount = parseObjectExpression();
    else if (!isStatementEnd())
        action->amount = parseExpression();

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<ScaleActionNode> Parser::parseScaleAction() {
    auto action = std::make_unique<ScaleActionNode>();

    consume("scale");

    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "{")
        action->amount = parseObjectExpression();
    else if (!isStatementEnd())
        action->amount = parseExpression();

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<ShowActionNode> Parser::parseShowAction() {
    auto action = std::make_unique<ShowActionNode>();

    consume("show");

    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<EditActionNode> Parser::parseEditAction() {
    auto action = std::make_unique<EditActionNode>();

    consume("edit");

    action->object = consume(TokenType::Identifier).value;

    if (peek().value == "{")
        action->properties = parseObjectExpression();

    if (peek().value == "over") {
        consume("over");
        action->duration = parseExpression();
    }

    if (peek().value == "using") {
        consume("using");
        action->interpolation = consume(TokenType::Identifier).value;
    }

    return action;
}

std::unique_ptr<WaitActionNode> Parser::parseWaitAction() {
    auto action = std::make_unique<WaitActionNode>();

    consume("wait");

    if (!isStatementEnd())
        action->duration = parseExpression();

    return action;
}

std::unique_ptr<PreloadDeclarationNode> Parser::parsePreloadDeclaration() {
    auto preload = std::make_unique<PreloadDeclarationNode>();

    consume("PRELOAD");
    consume(":");
    consume("{");

    while (peek().value != "}") {
        while (peek().type == TokenType::LineBreak ||
               peek().type == TokenType::Comma) {
            consume();
        }

        if (peek().value == "}")
            break;

        Token file = consume(TokenType::StringLiteral);

        auto resource = std::make_unique<ResourceDeclarationNode>();
        resource->path = file.value;

        size_t extensionPosition = file.value.find_last_of('.');

        if (extensionPosition == std::string::npos)
            throw std::runtime_error("Preloaded file must have an extension: " + file.value);

        std::string extension = file.value.substr(extensionPosition);

        if (extension != ".irt") {
            consume("as");

            Token alias = consume(TokenType::Identifier);
            resource->alias = alias.value;
        }

        preload->resources.push_back(std::move(resource));
    }

    consume("}");

    return preload;
}

std::unique_ptr<SettingsDeclarationNode> Parser::parseSettingsDeclaration() {
    auto settings = std::make_unique<SettingsDeclarationNode>();

    consume("SETTINGS");
    consume(":");

    settings->value = parseObjectExpression();

    if (peek().type == TokenType::Comma)
        consume(",");
    else
        consume(TokenType::LineBreak);

    return settings;
}

Token Parser::peek() {
    if (pos >= tokens.size())
        return Token(TokenType::FileEnd, "", 0, 0, 0);

    return tokens[pos];
}

Token Parser::peek(int distance) {
    if (pos + distance >= tokens.size())
        return Token(TokenType::FileEnd, "", 0, 0, 0);

    return tokens[pos + distance];
}

Token Parser::consume() {
    return tokens[pos++];
}

Token Parser::consume(std::string expected) {
    if (pos >= tokens.size() || tokens[pos].value != expected) {
        throw std::runtime_error(
            "Expected '" + expected + "', got '" +
            (pos < tokens.size() ? tokens[pos].value : "EOF") + "'"
        );
    }

    return tokens[pos++];
}

Token Parser::consume(TokenType expected) {
    if (pos >= tokens.size() || tokens[pos].type != expected) {
        throw std::runtime_error("Unexpected token type" + peek().value);
    }

    return tokens[pos++];
}
