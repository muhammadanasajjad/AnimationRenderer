#include "ASTPrinter.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr const char* Reset = "\x1b[0m";
constexpr const char* Cyan = "\x1b[36m";
constexpr const char* Yellow = "\x1b[33m";
constexpr const char* Green = "\x1b[32m";
constexpr const char* Magenta = "\x1b[35m";
constexpr const char* Dim = "\x1b[2m";

static std::string paint(const std::string& text, const char* color, bool useColor) {
    if (!useColor)
        return text;

    return std::string(color) + text + Reset;
}

static std::string typeName(ASTNodeType type) {
    switch (type) {
    case ASTNodeType::Program: return "Program";
    case ASTNodeType::PreloadDeclaration: return "PreloadDeclaration";
    case ASTNodeType::ResourceDeclaration: return "ResourceDeclaration";
    case ASTNodeType::SettingsDeclaration: return "SettingsDeclaration";
    case ASTNodeType::SceneDeclaration: return "SceneDeclaration";
    case ASTNodeType::ObjectDeclaration: return "ObjectDeclaration";
    case ASTNodeType::InterpolationDeclaration: return "InterpolationDeclaration";
    case ASTNodeType::PlayAction: return "PlayAction";
    case ASTNodeType::FadeAction: return "FadeAction";
    case ASTNodeType::RotateAction: return "RotateAction";
    case ASTNodeType::MoveAction: return "MoveAction";
    case ASTNodeType::ScaleAction: return "ScaleAction";
    case ASTNodeType::ShowAction: return "ShowAction";
    case ASTNodeType::EditAction: return "EditAction";
    case ASTNodeType::WaitAction: return "WaitAction";
    case ASTNodeType::IdentifierExpression: return "Identifier";
    case ASTNodeType::IntegerLiteral: return "IntegerLiteral";
    case ASTNodeType::FloatLiteral: return "FloatLiteral";
    case ASTNodeType::StringLiteral: return "StringLiteral";
    case ASTNodeType::BinaryExpression: return "BinaryExpression";
    case ASTNodeType::UnaryExpression: return "UnaryExpression";
    case ASTNodeType::UnitExpression: return "UnitExpression";
    case ASTNodeType::ObjectExpression: return "ObjectExpression";
    case ASTNodeType::TupleExpression: return "TupleExpression";
    case ASTNodeType::FunctionCallExpression: return "FunctionCallExpression";
    }

    return "Unknown";
}

static std::string formatNumber(double value) {
    std::ostringstream stream;

    if (value == static_cast<long long>(value))
        stream << static_cast<long long>(value);
    else
        stream << value;

    return stream.str();
}

struct ASTPrinterChild {
    std::string edge;
    const ASTNode* node;
};

static void addChild(std::vector<ASTPrinterChild>& children, const std::string& edge, const ASTNode* node) {
    if (node != nullptr)
        children.push_back({edge, node});
}

static std::vector<ASTPrinterChild> childrenOf(const ASTNode* node) {
    std::vector<ASTPrinterChild> children;

    switch (node->getType()) {
    case ASTNodeType::Program: {
        const auto* program = dynamic_cast<const ProgramNode*>(node);

        for (const auto& child : program->children)
            addChild(children, "", child.get());

        break;
    }
    case ASTNodeType::PreloadDeclaration: {
        const auto* preload = dynamic_cast<const PreloadDeclarationNode*>(node);

        for (const auto& resource : preload->resources)
            addChild(children, "", resource.get());

        break;
    }
    case ASTNodeType::SettingsDeclaration: {
        const auto* settings = dynamic_cast<const SettingsDeclarationNode*>(node);

        addChild(children, "value", settings->value.get());

        break;
    }
    case ASTNodeType::InterpolationDeclaration: {
        const auto* interpolation = dynamic_cast<const InterpolationDeclarationNode*>(node);

        addChild(children, "body", interpolation->function.get());

        break;
    }
    case ASTNodeType::ObjectDeclaration: {
        const auto* object = dynamic_cast<const ObjectDeclarationNode*>(node);

        addChild(children, "properties", object->properties.get());

        break;
    }
    case ASTNodeType::SceneDeclaration: {
        const auto* scene = dynamic_cast<const SceneDeclarationNode*>(node);

        for (const auto& child : scene->children)
            addChild(children, "", child.get());

        break;
    }
    case ASTNodeType::FadeAction: {
        const auto* action = dynamic_cast<const FadeActionNode*>(node);

        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::RotateAction: {
        const auto* action = dynamic_cast<const RotateActionNode*>(node);

        addChild(children, "amount", action->amount.get());
        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::MoveAction: {
        const auto* action = dynamic_cast<const MoveActionNode*>(node);

        addChild(children, "amount", action->amount.get());
        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::ScaleAction: {
        const auto* action = dynamic_cast<const ScaleActionNode*>(node);

        addChild(children, "amount", action->amount.get());
        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::ShowAction: {
        const auto* action = dynamic_cast<const ShowActionNode*>(node);

        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::EditAction: {
        const auto* action = dynamic_cast<const EditActionNode*>(node);

        addChild(children, "properties", action->properties.get());
        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::WaitAction: {
        const auto* action = dynamic_cast<const WaitActionNode*>(node);

        addChild(children, "duration", action->duration.get());

        break;
    }
    case ASTNodeType::BinaryExpression: {
        const auto* expression = dynamic_cast<const BinaryExpressionNode*>(node);

        addChild(children, "left", expression->left.get());
        addChild(children, "right", expression->right.get());

        break;
    }
    case ASTNodeType::UnaryExpression: {
        const auto* expression = dynamic_cast<const UnaryExpressionNode*>(node);

        addChild(children, "operand", expression->operand.get());

        break;
    }
    case ASTNodeType::UnitExpression: {
        const auto* expression = dynamic_cast<const UnitExpressionNode*>(node);

        addChild(children, "value", expression->value.get());

        break;
    }
    case ASTNodeType::ObjectExpression: {
        const auto* expression = dynamic_cast<const ObjectExpressionNode*>(node);

        std::vector<std::string> keys;

        for (const auto& [key, value] : expression->properties)
            keys.push_back(key);

        std::sort(keys.begin(), keys.end());

        for (const auto& key : keys)
            addChild(children, key, expression->properties.at(key).get());

        break;
    }
    case ASTNodeType::TupleExpression: {
        const auto* expression = dynamic_cast<const TupleExpressionNode*>(node);

        for (size_t i = 0; i < expression->elements.size(); i++)
            addChild(children, "[" + std::to_string(i) + "]", expression->elements[i].get());

        break;
    }
    case ASTNodeType::FunctionCallExpression: {
        const auto* expression = dynamic_cast<const FunctionCallExpressionNode*>(node);

        addChild(children, "callee", expression->callee.get());

        for (size_t i = 0; i < expression->arguments.size(); i++)
            addChild(children, "arg[" + std::to_string(i) + "]", expression->arguments[i].get());

        break;
    }
    default:
        break;
    }

    return children;
}

static std::string nodeLabel(const ASTNode* node, bool useColor) {
    std::string label = paint(typeName(node->getType()), Cyan, useColor);

    const auto value = [&](const std::string& text) {
        return paint(text, Yellow, useColor);
    };
    const auto keyword = [&](const std::string& text) {
        return paint(text, Green, useColor);
    };
    const auto extra = [&](const std::string& text) {
        return paint(text, Magenta, useColor);
    };

    switch (node->getType()) {
    case ASTNodeType::ResourceDeclaration: {
        const auto* resource = dynamic_cast<const ResourceDeclarationNode*>(node);

        label += " " + value(resource->path);

        if (!resource->alias.empty())
            label += " as " + value(resource->alias);

        break;
    }
    case ASTNodeType::InterpolationDeclaration: {
        const auto* interpolation = dynamic_cast<const InterpolationDeclarationNode*>(node);

        label += " " + value(interpolation->name);
        label += " " + keyword(interpolation->functionName + "(" + interpolation->parameter + ")");

        break;
    }
    case ASTNodeType::ObjectDeclaration: {
        const auto* object = dynamic_cast<const ObjectDeclarationNode*>(node);

        label += " " + keyword(object->objectType) + " " + value(object->name);

        break;
    }
    case ASTNodeType::SceneDeclaration: {
        const auto* scene = dynamic_cast<const SceneDeclarationNode*>(node);

        label += " " + value(scene->name);

        break;
    }
    case ASTNodeType::PlayAction: {
        const auto* action = dynamic_cast<const PlayActionNode*>(node);

        label += " " + value(action->scene);

        break;
    }
    case ASTNodeType::FadeAction: {
        const auto* action = dynamic_cast<const FadeActionNode*>(node);

        label += " " + keyword(action->isFadeIn ? "fadeIn" : "fadeOut");
        label += " " + value(action->object);

        if (!action->interpolation.empty())
            label += " using " + extra(action->interpolation);

        break;
    }
    case ASTNodeType::RotateAction:
    case ASTNodeType::MoveAction:
    case ASTNodeType::ScaleAction: {
        std::string object;
        std::string interpolation;

        if (node->getType() == ASTNodeType::RotateAction) {
            const auto* action = dynamic_cast<const RotateActionNode*>(node);
            object = action->object;
            interpolation = action->interpolation;
        } else if (node->getType() == ASTNodeType::MoveAction) {
            const auto* action = dynamic_cast<const MoveActionNode*>(node);
            object = action->object;
            interpolation = action->interpolation;
        } else {
            const auto* action = dynamic_cast<const ScaleActionNode*>(node);
            object = action->object;
            interpolation = action->interpolation;
        }

        label += " " + value(object);

        if (!interpolation.empty())
            label += " using " + extra(interpolation);

        break;
    }
    case ASTNodeType::ShowAction: {
        const auto* action = dynamic_cast<const ShowActionNode*>(node);

        label += " " + value(action->object);

        if (!action->interpolation.empty())
            label += " using " + extra(action->interpolation);

        break;
    }
    case ASTNodeType::EditAction: {
        const auto* action = dynamic_cast<const EditActionNode*>(node);

        label += " " + value(action->object);

        if (!action->interpolation.empty())
            label += " using " + extra(action->interpolation);

        break;
    }
    case ASTNodeType::IdentifierExpression: {
        const auto* identifier = dynamic_cast<const IdentifierNode*>(node);

        label += " " + value(identifier->name);

        break;
    }
    case ASTNodeType::IntegerLiteral: {
        const auto* literal = dynamic_cast<const IntegerLiteralNode*>(node);

        label += " " + value(formatNumber(literal->value));

        break;
    }
    case ASTNodeType::FloatLiteral: {
        const auto* literal = dynamic_cast<const FloatLiteralNode*>(node);

        label += " " + value(formatNumber(literal->value));

        break;
    }
    case ASTNodeType::StringLiteral: {
        const auto* literal = dynamic_cast<const StringLiteralNode*>(node);

        label += " " + value("\"" + literal->value + "\"");

        break;
    }
    case ASTNodeType::BinaryExpression: {
        const auto* expression = dynamic_cast<const BinaryExpressionNode*>(node);

        label += " " + keyword(expression->operation);

        break;
    }
    case ASTNodeType::UnaryExpression: {
        const auto* expression = dynamic_cast<const UnaryExpressionNode*>(node);

        label += " " + keyword(expression->operation);

        break;
    }
    case ASTNodeType::UnitExpression: {
        const auto* expression = dynamic_cast<const UnitExpressionNode*>(node);

        label += " " + keyword(expression->unit);

        break;
    }
    default:
        break;
    }

    return label;
}

static void printNode(
    const ASTNode* node,
    std::ostream& out,
    const std::string& prefix,
    bool isLast,
    bool isRoot,
    bool useColor,
    const std::string& edge
) {
    std::string connector = isRoot ? "" : (isLast ? "└── " : "├── ");

    out << prefix << connector;

    if (!edge.empty())
        out << paint(edge, Dim, useColor) << ": ";

    out << nodeLabel(node, useColor) << '\n';

    const auto children = childrenOf(node);

    std::string childPrefix = prefix;

    if (!isRoot)
        childPrefix += isLast ? "    " : "│   ";

    for (size_t i = 0; i < children.size(); i++) {
        printNode(
            children[i].node,
            out,
            childPrefix,
            i + 1 == children.size(),
            false,
            useColor,
            children[i].edge
        );
    }
}

void ASTPrinter::print(const ASTNode* root) {
    print(root, std::cout, true);
}

void ASTPrinter::print(const ASTNode* root, std::ostream& out, bool useColor) {
    if (root == nullptr) {
        out << "(null AST)\n";
        return;
    }

    printNode(root, out, "", true, true, useColor, "");
}
