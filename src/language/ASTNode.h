
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

enum class ASTNodeType {
    Program,

    PreloadDeclaration,
    ResourceDeclaration,
    SettingsDeclaration,
    SceneDeclaration,
    ObjectDeclaration,
    InterpolationDeclaration,

    PlayAction,
    FadeAction,
    RotateAction,
    MoveAction,
    ScaleAction,
    ShowAction,
    EditAction,
    WaitAction,

    IdentifierExpression,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    BinaryExpression,
    UnaryExpression,
    UnitExpression,
    ObjectExpression,
    TupleExpression,
    FunctionCallExpression
};

class ASTNode {
public:
    virtual ASTNodeType getType() const = 0;
    virtual ~ASTNode() = default;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> children;

    ASTNodeType getType() const override {
        return ASTNodeType::Program;
    }
};

class DeclarationNode : public ASTNode {
public:
    std::string name;
};

class ExpressionNode : public ASTNode {
};

class IdentifierNode : public ExpressionNode {
public:
    std::string name;

    ASTNodeType getType() const override {
        return ASTNodeType::IdentifierExpression;
    }
};

class BinaryExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
    std::string operation;

    ASTNodeType getType() const override {
        return ASTNodeType::BinaryExpression;
    }
};

class UnaryExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> operand;
    std::string operation;

    ASTNodeType getType() const override {
        return ASTNodeType::UnaryExpression;
    }
};

class UnitExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> value;
    std::string unit;

    ASTNodeType getType() const override {
        return ASTNodeType::UnitExpression;
    }
};

class FloatLiteralNode : public ExpressionNode {
public:
    double value;

    ASTNodeType getType() const override {
        return ASTNodeType::FloatLiteral;
    }
};

class IntegerLiteralNode : public ExpressionNode {
public:
    double value;
    bool isInteger;

    ASTNodeType getType() const override {
        return ASTNodeType::IntegerLiteral;
    }
};

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;

    ASTNodeType getType() const override {
        return ASTNodeType::StringLiteral;
    }
};

class ObjectExpressionNode : public ExpressionNode {
public:
    std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> properties;

    ASTNodeType getType() const override {
        return ASTNodeType::ObjectExpression;
    }
};

class TupleExpressionNode : public ExpressionNode {
public:
    std::vector<std::unique_ptr<ExpressionNode>> elements;

    ASTNodeType getType() const override {
        return ASTNodeType::TupleExpression;
    }
};

class FunctionCallExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> callee;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;

    ASTNodeType getType() const override {
        return ASTNodeType::FunctionCallExpression;
    }
};

class ResourceDeclarationNode : public ASTNode {
public:
    std::string path;
    std::string alias;

    ASTNodeType getType() const override {
        return ASTNodeType::ResourceDeclaration;
    }
};

class PreloadDeclarationNode : public DeclarationNode {
public:
    std::vector<std::unique_ptr<ResourceDeclarationNode>> resources;

    ASTNodeType getType() const override {
        return ASTNodeType::PreloadDeclaration;
    }
};

class SettingsDeclarationNode : public DeclarationNode {
public:
    std::unique_ptr<ObjectExpressionNode> value;

    ASTNodeType getType() const override {
        return ASTNodeType::SettingsDeclaration;
    }
};

class InterpolationDeclarationNode : public DeclarationNode {
public:
    std::string functionName;
    std::string parameter;
    std::unique_ptr<ExpressionNode> function;

    ASTNodeType getType() const override {
        return ASTNodeType::InterpolationDeclaration;
    }
};

class ObjectDeclarationNode : public DeclarationNode {
public:
    std::string objectType;
    std::unique_ptr<ObjectExpressionNode> properties;

    ASTNodeType getType() const override {
        return ASTNodeType::ObjectDeclaration;
    }
};

class SceneDeclarationNode : public DeclarationNode {
public:
    std::vector<std::unique_ptr<ASTNode>> children;

    ASTNodeType getType() const override {
        return ASTNodeType::SceneDeclaration;
    }
};

class PlayActionNode : public ASTNode {
public:
    std::string scene;

    ASTNodeType getType() const override {
        return ASTNodeType::PlayAction;
    }
};

class FadeActionNode : public ASTNode {
public:
    std::string object;
    bool isFadeIn;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::FadeAction;
    }
};

class RotateActionNode : public ASTNode {
public:
    std::string object;
    std::unique_ptr<ExpressionNode> amount;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::RotateAction;
    }
};

class MoveActionNode : public ASTNode {
public:
    std::string object;
    std::unique_ptr<ExpressionNode> amount;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::MoveAction;
    }
};

class ScaleActionNode : public ASTNode {
public:
    std::string object;
    std::unique_ptr<ExpressionNode> amount;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::ScaleAction;
    }
};

class ShowActionNode : public ASTNode {
public:
    std::string object;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::ShowAction;
    }
};

class EditActionNode : public ASTNode {
public:
    std::string object;
    std::unique_ptr<ExpressionNode> properties;
    std::unique_ptr<ExpressionNode> duration;
    std::string interpolation;

    ASTNodeType getType() const override {
        return ASTNodeType::EditAction;
    }
};

class WaitActionNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> duration;

    ASTNodeType getType() const override {
        return ASTNodeType::WaitAction;
    }
};
