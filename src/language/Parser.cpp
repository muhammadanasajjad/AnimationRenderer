#include "Parser.h"

#include <iostream>
#include <unordered_set>

#include "ASTNode.h"
#include "ParseError.h"

std::string describeToken(Token token) {
    if (token.type == TokenType::FileEnd) {
        return "end of file";
    }

    if (token.type == TokenType::LineBreak) {
        return "end of line";
    }

    if (token.type == TokenType::WhiteSpace) {
        return "whitespace";
    }

    return "'" + token.value + "'";
}

std::string describeTokenType(TokenType type) {
    switch (type) {
        case TokenType::FileEnd:
            return "end of file";
        case TokenType::LineBreak:
            return "end of line";
        case TokenType::WhiteSpace:
            return "whitespace";
        case TokenType::Colon:
            return "':'";
        case TokenType::OpenCurlyBrace:
            return "'{'";
        case TokenType::CloseCurlyBrace:
            return "'}'";
        case TokenType::OpenParenthesis:
            return "'('";
        case TokenType::CloseParenthesis:
            return "')'";
        case TokenType::Comma:
            return "','";
        case TokenType::Equals:
            return "'='";
        case TokenType::Caret:
            return "'^'";
        case TokenType::Asterisk:
            return "'*'";
        case TokenType::ForwardSlash:
            return "'/'";
        case TokenType::Plus:
            return "'+'";
        case TokenType::Minus:
            return "'-'";
        case TokenType::IntegerLiteral:
            return "integer literal";
        case TokenType::FloatLiteral:
            return "float literal";
        case TokenType::StringLiteral:
            return "string literal";
        case TokenType::ObjectType:
            return "object type";
        case TokenType::Identifier:
            return "identifier";
        default:
            return "token";
    }
}

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
    
    throw ParseError{peek(), "Expected PRELOAD, SETTINGS, SCENE, or play, got " + describeToken(peek())};
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

    throw ParseError{peek(), "Expected expression, got " + describeToken(peek())};
}

std::unique_ptr<IntegerLiteralNode> Parser::parseIntegerLiteral() {
    Token token = consume();

    auto literal = std::make_unique<IntegerLiteralNode>();
    literal->token = token;
    literal->value = std::stod(token.value);
    literal->isInteger = true;

    return literal;
}

std::unique_ptr<FloatLiteralNode> Parser::parseFloatLiteral() {
    Token token = consume();

    auto literal = std::make_unique<FloatLiteralNode>();
    literal->token = token;
    literal->value = std::stod(token.value);

    return literal;
}

std::unique_ptr<StringLiteralNode> Parser::parseStringLiteral() {
    Token token = consume();

    auto literal = std::make_unique<StringLiteralNode>();
    literal->token = token;
    literal->value = token.value;

    return literal;
}

std::unique_ptr<IdentifierNode> Parser::parseIdentifierExpression() {
    Token token = consume();

    auto identifier = std::make_unique<IdentifierNode>();
    identifier->token = token;
    identifier->name = token.value;

    return identifier;
}

std::unique_ptr<ObjectExpressionNode> Parser::parseObjectExpression() {
    auto objectExpression = std::make_unique<ObjectExpressionNode>();

    objectExpression->token = consume("{");

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
            throw ParseError{peek(), "Expected identifier, got " + describeToken(peek())};

        Token parameterName = consume();

        consume(":");

        objectExpression->properties[parameterName.value] = parseExpression();
    }

    consume("}");

    return objectExpression;
}

std::unique_ptr<TupleExpressionNode> Parser::parseTupleExpression() {
    auto tuple = std::make_unique<TupleExpressionNode>();

    tuple->token = consume("{");

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

    scene->token = consume("SCENE");

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

    interpolation->token = consume("INTERPOLATION");

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

    object->token = peek();
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

    throw ParseError{peek(), "Expected object type, or action, got " + describeToken(peek())};
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

    throw ParseError{peek(), "Expected fadeIn, fadeOut, rotate, move, scale, show, edit, or wait, got " + describeToken(peek())};
}

std::unique_ptr<PlayActionNode> Parser::parsePlayAction() {
    auto action = std::make_unique<PlayActionNode>();

    action->token = consume("play");

    action->scene = consume(TokenType::Identifier).value;

    return action;
}

std::unique_ptr<FadeActionNode> Parser::parseFadeAction() {
    auto action = std::make_unique<FadeActionNode>();

    Token keyword = consume();

    action->token = keyword;
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

    action->token = peek();
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

    action->token = peek();
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

    action->token = peek();
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

    action->token = peek();
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

    action->token = peek();
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

    action->token = peek();
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
            throw ParseError{file, "Expected file path with an extension, got " + describeToken(file)};

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
    Token token = peek();
    if (pos >= tokens.size() || token.value != expected) {
        throw ParseError{token, "Expected '" + expected + "', got " + describeToken(token)};
    }

    return tokens[pos++];
}

Token Parser::consume(TokenType expected) {
    Token token = peek();
    if (pos >= tokens.size() || token.type != expected) {
        throw ParseError{token, "Expected " + describeTokenType(expected) + ", got " + describeToken(token)};
    }

    return tokens[pos++];
}
