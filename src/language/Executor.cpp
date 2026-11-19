#include "Executor.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "../scene/Shapes.h"
#include "../scene/Shapes2D.h"
#include "../scene/Shapes3D.h"

class EvalValue {
public:
    using Variables = std::unordered_map<std::string, float>;
    using Interpolations = std::unordered_map<std::string, std::function<float(float)>>;

    enum class Kind { None, Number, Vector, Color, String, Boolean };

    struct EvalContext {
        Variables* variables = nullptr;
        Interpolations* interpolations = nullptr;
    };

    Kind kind = Kind::None;
    double number = 0.0;
    glm::vec3 vector = glm::vec3(0.0f);
    glm::vec4 color = glm::vec4(0.0f);
    std::string text;

    static EvalValue fromNumber(double value) {
        EvalValue result;
        result.kind = Kind::Number;
        result.number = value;
        return result;
    }

    static EvalValue fromVector(glm::vec3 value) {
        EvalValue result;
        result.kind = Kind::Vector;
        result.vector = value;
        return result;
    }

    static EvalValue fromColor(glm::vec4 value) {
        EvalValue result;
        result.kind = Kind::Color;
        result.color = value;
        return result;
    }

    static EvalValue fromString(std::string value) {
        EvalValue result;
        result.kind = Kind::String;
        result.text = value;
        return result;
    }

    double asNumber() {
        if (kind == Kind::Number || kind == Kind::Boolean)
            return number;
        if (kind == Kind::Vector)
            return vector.x;
        if (kind == Kind::Color)
            return color.r;
        return 0.0;
    }

    glm::vec3 asVector() {
        if (kind == Kind::Vector)
            return vector;
        if (kind == Kind::Color)
            return glm::vec3(color);
        return glm::vec3(static_cast<float>(number));
    }

    glm::vec4 asColor() {
        if (kind == Kind::Color)
            return color;
        if (kind == Kind::Vector)
            return glm::vec4(vector, 1.0f);
        return glm::vec4(static_cast<float>(number));
    }

    static EvalValue evaluateExpression(ExpressionNode* node, EvalContext& context);
    static EvalValue evaluateCall(FunctionCallExpressionNode* node, EvalContext& context);
    static bool isShapeNumberKey(std::string key);
    static float getShapeNumber(Object* object, std::string key);
    static void setShapeNumber(Object* object, std::string key, float value);
};

EvalValue EvalValue::evaluateCall(FunctionCallExpressionNode* node, EvalValue::EvalContext& context) {
    std::string name;

    if (node->callee && node->callee->getType() == ASTNodeType::IdentifierExpression) {
        name = static_cast<IdentifierNode*>(node->callee.get())->name;
    }

    if (context.interpolations) {
        auto it = context.interpolations->find(name);

        if (it != context.interpolations->end()) {
            float t = 0.0f;

            if (!node->arguments.empty()) {
                t = static_cast<float>(evaluateExpression(node->arguments[0].get(), context).asNumber());
            }

            return EvalValue::fromNumber(it->second(t));
        }
    }

    std::vector<EvalValue> arguments;

    for (std::unique_ptr<ExpressionNode>& argument : node->arguments) {
        arguments.push_back(evaluateExpression(argument.get(), context));
    }

    double a = arguments.size() > 0 ? arguments[0].asNumber() : 0.0;
    double b = arguments.size() > 1 ? arguments[1].asNumber() : 0.0;
    double c = arguments.size() > 2 ? arguments[2].asNumber() : 0.0;

    if (name == "sin") return EvalValue::fromNumber(std::sin(a));
    if (name == "cos") return EvalValue::fromNumber(std::cos(a));
    if (name == "tan") return EvalValue::fromNumber(std::tan(a));
    if (name == "asin") return EvalValue::fromNumber(std::asin(a));
    if (name == "acos") return EvalValue::fromNumber(std::acos(a));
    if (name == "atan") return EvalValue::fromNumber(std::atan(a));
    if (name == "atan2") return EvalValue::fromNumber(std::atan2(a, b));
    if (name == "sqrt") return EvalValue::fromNumber(std::sqrt(a));
    if (name == "abs") return EvalValue::fromNumber(std::abs(a));
    if (name == "exp") return EvalValue::fromNumber(std::exp(a));
    if (name == "log") return EvalValue::fromNumber(std::log(a));
    if (name == "log10") return EvalValue::fromNumber(std::log10(a));
    if (name == "pow") return EvalValue::fromNumber(std::pow(a, b));
    if (name == "min") return EvalValue::fromNumber(std::min(a, b));
    if (name == "max") return EvalValue::fromNumber(std::max(a, b));
    if (name == "floor") return EvalValue::fromNumber(std::floor(a));
    if (name == "ceil") return EvalValue::fromNumber(std::ceil(a));
    if (name == "round") return EvalValue::fromNumber(std::round(a));
    if (name == "sign") return EvalValue::fromNumber(a < 0.0 ? -1.0 : (a > 0.0 ? 1.0 : 0.0));
    if (name == "radians") return EvalValue::fromNumber(glm::radians(static_cast<float>(a)));
    if (name == "degrees") return EvalValue::fromNumber(glm::degrees(static_cast<float>(a)));
    if (name == "clamp") return EvalValue::fromNumber(std::clamp(a, b, c));
    if (name == "mod") return EvalValue::fromNumber(b == 0.0 ? 0.0 : std::fmod(a, b));

    return EvalValue::fromNumber(0.0);
}

EvalValue EvalValue::evaluateExpression(ExpressionNode* node, EvalValue::EvalContext& context) {
    if (!node) {
        return EvalValue();
    }

    ASTNodeType type = node->getType();

    if (type == ASTNodeType::IntegerLiteral) {
        return EvalValue::fromNumber(static_cast<IntegerLiteralNode*>(node)->value);
    } else if (type == ASTNodeType::FloatLiteral) {
        return EvalValue::fromNumber(static_cast<FloatLiteralNode*>(node)->value);
    } else if (type == ASTNodeType::StringLiteral) {
        return EvalValue::fromString(static_cast<StringLiteralNode*>(node)->value);
    } else if (type == ASTNodeType::IdentifierExpression) {
        std::string name = static_cast<IdentifierNode*>(node)->name;

        if (context.variables) {
            auto it = context.variables->find(name);

            if (it != context.variables->end()) {
                return EvalValue::fromNumber(it->second);
            }
        }

        if (name == "pi") return EvalValue::fromNumber(glm::pi<double>());
        if (name == "e") return EvalValue::fromNumber(std::exp(1.0));

        return EvalValue::fromNumber(0.0);
    } else if (type == ASTNodeType::UnaryExpression) {
        UnaryExpressionNode* unary = static_cast<UnaryExpressionNode*>(node);
        EvalValue value = evaluateExpression(unary->operand.get(), context);

        if (unary->operation == "-") {
            if (value.kind == EvalValue::Kind::Vector)
                return EvalValue::fromVector(-value.vector);
            if (value.kind == EvalValue::Kind::Color)
                return EvalValue::fromColor(-value.color);
            return EvalValue::fromNumber(-value.asNumber());
        }

        return value;
    } else if (type == ASTNodeType::BinaryExpression) {
        BinaryExpressionNode* binary = static_cast<BinaryExpressionNode*>(node);
        EvalValue left = evaluateExpression(binary->left.get(), context);
        EvalValue right = evaluateExpression(binary->right.get(), context);
        std::string operation = binary->operation;

        if (left.kind == EvalValue::Kind::Vector || right.kind == EvalValue::Kind::Vector ||
            left.kind == EvalValue::Kind::Color || right.kind == EvalValue::Kind::Color) {
            glm::vec3 l = left.asVector();
            glm::vec3 r = right.asVector();

            if (operation == "+") return EvalValue::fromVector(l + r);
            if (operation == "-") return EvalValue::fromVector(l - r);
            if (operation == "*") return EvalValue::fromVector(l * r);
            if (operation == "/") return EvalValue::fromVector(l / glm::max(r, glm::vec3(1e-9f)));

            return EvalValue::fromVector(glm::vec3(0.0f));
        }

        double l = left.asNumber();
        double r = right.asNumber();

        if (operation == "+") return EvalValue::fromNumber(l + r);
        if (operation == "-") return EvalValue::fromNumber(l - r);
        if (operation == "*") return EvalValue::fromNumber(l * r);
        if (operation == "/") return EvalValue::fromNumber(r == 0.0 ? 0.0 : l / r);
        if (operation == "^") return EvalValue::fromNumber(std::pow(l, r));

        return EvalValue::fromNumber(0.0);
    } else if (type == ASTNodeType::UnitExpression) {
        UnitExpressionNode* unit = static_cast<UnitExpressionNode*>(node);
        EvalValue value = evaluateExpression(unit->value.get(), context);

        if (unit->unit == "ms") return EvalValue::fromNumber(value.asNumber() / 1000.0);
        if (unit->unit == "fps") return EvalValue::fromNumber(value.asNumber() == 0.0 ? 0.0 : 1.0 / value.asNumber());
        if (unit->unit == "deg") return EvalValue::fromNumber(glm::radians(static_cast<float>(value.asNumber())));
        if (unit->unit == "s" || unit->unit == "rad") return value;

        return value;
    } else if (type == ASTNodeType::TupleExpression) {
        TupleExpressionNode* tuple = static_cast<TupleExpressionNode*>(node);

        if (tuple->elements.empty())
            return EvalValue();

        glm::vec3 vector = glm::vec3(0.0f);

        for (size_t i = 0; i < tuple->elements.size() && i < 3; i++) {
            vector[static_cast<int>(i)] = static_cast<float>(evaluateExpression(tuple->elements[i].get(), context).asNumber());
        }

        if (tuple->elements.size() == 4) {
            float w = static_cast<float>(evaluateExpression(tuple->elements[3].get(), context).asNumber());
            return EvalValue::fromColor(glm::vec4(vector, w));
        }

        if (tuple->elements.size() == 1)
            return EvalValue::fromNumber(vector.x);

        return EvalValue::fromVector(vector);
    } else if (type == ASTNodeType::FunctionCallExpression) {
        return evaluateCall(static_cast<FunctionCallExpressionNode*>(node), context);
    }

    return EvalValue();
}

bool EvalValue::isShapeNumberKey(std::string key) {
    return key == "size" || key == "sideLength" ||
           key == "width" || key == "height" || key == "depth" ||
           key == "radius" || key == "radiusX" || key == "radiusY" ||
           key == "strokeWidth";
}

float EvalValue::getShapeNumber(Object* object, std::string key) {
    Square* square = dynamic_cast<Square*>(object);

    if (square && (key == "size" || key == "sideLength"))
        return square->sideLength;

    Rectangle* rectangle = dynamic_cast<Rectangle*>(object);

    if (rectangle) {
        if (key == "width") return rectangle->width;
        if (key == "height") return rectangle->height;
    }

    Ellipse* ellipse = dynamic_cast<Ellipse*>(object);

    if (ellipse) {
        if (key == "radius") return ellipse->radiusX;
        if (key == "radiusX") return ellipse->radiusX;
        if (key == "radiusY") return ellipse->radiusY;
    }

    Cube* cube = dynamic_cast<Cube*>(object);

    if (cube && (key == "size" || key == "sideLength" || key == "width" || key == "height" || key == "depth"))
        return cube->sideLength;

    Cuboid* cuboid = dynamic_cast<Cuboid*>(object);

    if (cuboid) {
        if (key == "width") return cuboid->width;
        if (key == "height") return cuboid->height;
        if (key == "depth") return cuboid->depth;
    }

    Cylinder* cylinder = dynamic_cast<Cylinder*>(object);

    if (cylinder) {
        if (key == "radius") return cylinder->radius;
        if (key == "height") return cylinder->height;
    }

    Sphere* sphere = dynamic_cast<Sphere*>(object);

    if (sphere && key == "radius")
        return sphere->radius;

    Shapes* shape = dynamic_cast<Shapes*>(object);

    if (shape && key == "strokeWidth")
        return shape->stroke.width;

    return 0.0f;
}

void EvalValue::setShapeNumber(Object* object, std::string key, float value) {
    Square* square = dynamic_cast<Square*>(object);

    if (square && (key == "size" || key == "sideLength")) {
        square->sideLength = value;
        square->rebuildOutline();
        return;
    }

    Rectangle* rectangle = dynamic_cast<Rectangle*>(object);

    if (rectangle) {
        if (key == "width") {
            rectangle->width = value;
            rectangle->rebuildOutline();
            return;
        }
        if (key == "height") {
            rectangle->height = value;
            rectangle->rebuildOutline();
            return;
        }
    }

    Ellipse* ellipse = dynamic_cast<Ellipse*>(object);

    if (ellipse) {
        if (key == "radius" || key == "radiusX") {
            ellipse->radiusX = value;
            ellipse->rebuildOutline();
            return;
        }
        if (key == "radiusY") {
            ellipse->radiusY = value;
            ellipse->rebuildOutline();
            return;
        }
    }

    Cube* cube = dynamic_cast<Cube*>(object);

    if (cube && (key == "size" || key == "sideLength" || key == "width" || key == "height" || key == "depth")) {
        cube->sideLength = value;
        cube->rebuildMesh();
        return;
    }

    Cuboid* cuboid = dynamic_cast<Cuboid*>(object);

    if (cuboid) {
        if (key == "width") {
            cuboid->width = value;
            cuboid->rebuildMesh();
            return;
        }
        if (key == "height") {
            cuboid->height = value;
            cuboid->rebuildMesh();
            return;
        }
        if (key == "depth") {
            cuboid->depth = value;
            cuboid->rebuildMesh();
            return;
        }
    }

    Cylinder* cylinder = dynamic_cast<Cylinder*>(object);

    if (cylinder) {
        if (key == "radius") {
            cylinder->radius = value;
            cylinder->rebuildMesh();
            return;
        }
        if (key == "height") {
            cylinder->height = value;
            cylinder->rebuildMesh();
            return;
        }
    }

    Sphere* sphere = dynamic_cast<Sphere*>(object);

    if (sphere && key == "radius") {
        sphere->radius = value;
        sphere->rebuildMesh();
        return;
    }

    Shapes* shape = dynamic_cast<Shapes*>(object);

    if (!shape)
        return;

    if (key == "strokeWidth") {
        shape->stroke.width = value;
    } else if (key == "fill") {
        shape->fill.enabled = value != 0.0f;
    } else if (key == "stroke") {
        shape->stroke.enabled = value != 0.0f;
    }
}

Executor::SavedObjectState Executor::captureObjectState(Object* object) {
    SavedObjectState state;

    if (!object) {
        return state;
    }

    state.transform = object->transform;
    state.colourMask = object->colourMask;

    Shapes* shape = dynamic_cast<Shapes*>(object);

    if (shape) {
        state.strokeWidth = shape->stroke.width;
        state.fillEnabled = shape->fill.enabled;
        state.strokeEnabled = shape->stroke.enabled;
    }

    Square* square = dynamic_cast<Square*>(object);

    if (square) {
        state.sideLength = square->sideLength;
    }

    Rectangle* rectangle = dynamic_cast<Rectangle*>(object);

    if (rectangle) {
        state.width = rectangle->width;
        state.height = rectangle->height;
    }

    Ellipse* ellipse = dynamic_cast<Ellipse*>(object);

    if (ellipse) {
        state.radiusX = ellipse->radiusX;
        state.radiusY = ellipse->radiusY;
    }

    Cube* cube = dynamic_cast<Cube*>(object);

    if (cube) {
        state.sideLength = cube->sideLength;
    }

    Cuboid* cuboid = dynamic_cast<Cuboid*>(object);

    if (cuboid) {
        state.width = cuboid->width;
        state.height = cuboid->height;
        state.depth = cuboid->depth;
    }

    Cylinder* cylinder = dynamic_cast<Cylinder*>(object);

    if (cylinder) {
        state.radiusX = cylinder->radius;
        state.height = cylinder->height;
    }

    Sphere* sphere = dynamic_cast<Sphere*>(object);

    if (sphere) {
        state.radiusX = sphere->radius;
    }

    return state;
}

void Executor::restoreObjectState(Object* object, SavedObjectState& state) {
    if (!object) {
        return;
    }

    object->transform = state.transform;
    object->colourMask = state.colourMask;

    Shapes* shape = dynamic_cast<Shapes*>(object);

    if (shape) {
        shape->stroke.width = state.strokeWidth;
        shape->fill.enabled = state.fillEnabled;
        shape->stroke.enabled = state.strokeEnabled;
    }

    Square* square = dynamic_cast<Square*>(object);

    if (square) {
        square->sideLength = state.sideLength;
        square->rebuildOutline();
    }

    Rectangle* rectangle = dynamic_cast<Rectangle*>(object);

    if (rectangle) {
        rectangle->width = state.width;
        rectangle->height = state.height;
        rectangle->rebuildOutline();
    }

    Ellipse* ellipse = dynamic_cast<Ellipse*>(object);

    if (ellipse) {
        ellipse->radiusX = state.radiusX;
        ellipse->radiusY = state.radiusY;
        ellipse->rebuildOutline();
    }

    Cube* cube = dynamic_cast<Cube*>(object);

    if (cube) {
        cube->sideLength = state.sideLength;
        cube->rebuildMesh();
    }

    Cuboid* cuboid = dynamic_cast<Cuboid*>(object);

    if (cuboid) {
        cuboid->width = state.width;
        cuboid->height = state.height;
        cuboid->depth = state.depth;
        cuboid->rebuildMesh();
    }

    Cylinder* cylinder = dynamic_cast<Cylinder*>(object);

    if (cylinder) {
        cylinder->radius = state.radiusX;
        cylinder->height = state.height;
        cylinder->rebuildMesh();
    }

    Sphere* sphere = dynamic_cast<Sphere*>(object);

    if (sphere) {
        sphere->radius = state.radiusX;
        sphere->rebuildMesh();
    }
}

Executor::Executor(Objects& objects)
    : objects(objects), timeline(std::make_unique<Timeline>(objects)) {
}

void Executor::execute(ProgramNode* program) {
    if (!program) {
        return;
    }

    auto scenes = std::unordered_map<std::string, SceneDeclarationNode*>();

    for (std::unique_ptr<ASTNode>& child : program->children) {
        if (child->getType() == ASTNodeType::SceneDeclaration) {
            SceneDeclarationNode* scene = static_cast<SceneDeclarationNode*>(child.get());
            scenes[scene->name] = scene;
        }
    }

    bool played = false;

    for (std::unique_ptr<ASTNode>& child : program->children) {
        if (child->getType() == ASTNodeType::PlayAction) {
            PlayActionNode* play = static_cast<PlayActionNode*>(child.get());
            auto it = scenes.find(play->scene);

            if (it != scenes.end()) {
                execute(it->second);
                played = true;
            }
        }
    }

    if (!played && !scenes.empty()) {
        execute(scenes.begin()->second);
    }
}

void Executor::execute(SceneDeclarationNode* scene) {
    if (!scene) {
        return;
    }

    currentTime = 0.0f;
    pendingEndTime = 0.0f;
    objectIndices.clear();
    interpolations.clear();
    timeline->clear();

    savedStates.clear();

    for (std::unique_ptr<Object>& object : objects) {
        savedStates.push_back(captureObjectState(object.get()));
    }

    for (std::unique_ptr<ASTNode>& child : scene->children) {
        if (child->getType() == ASTNodeType::ObjectDeclaration) {
            executeObjectDeclaration(static_cast<ObjectDeclarationNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::InterpolationDeclaration) {
            executeInterpolationDeclaration(static_cast<InterpolationDeclarationNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::FadeAction) {
            executeFadeAction(static_cast<FadeActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::RotateAction) {
            executeRotateAction(static_cast<RotateActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::MoveAction) {
            executeMoveAction(static_cast<MoveActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::ScaleAction) {
            executeScaleAction(static_cast<ScaleActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::ShowAction) {
            executeShowAction(static_cast<ShowActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::EditAction) {
            executeEditAction(static_cast<EditActionNode*>(child.get()));
        } else if (child->getType() == ASTNodeType::WaitAction) {
            executeWaitAction(static_cast<WaitActionNode*>(child.get()));
        }
    }

    for (size_t i = 0; i < objects.size() && i < savedStates.size(); i++) {
        restoreObjectState(objects[i].get(), savedStates[i]);
    }

    timeline->reset();
}

void Executor::executeObjectDeclaration(ObjectDeclarationNode* node) {
    if (!node) {
        return;
    }

    auto object = createObject(node->objectType, node->properties.get());

    if (!object) {
        return;
    }

    int index = static_cast<int>(objects.size());
    objectIndices[node->name] = index;
    objects.push_back(std::move(object));
    savedStates.push_back(captureObjectState(objects.back().get()));
}

void Executor::executeInterpolationDeclaration(InterpolationDeclarationNode* node) {
    if (!node || !node->function) {
        return;
    }

    interpolations[node->name] = [this, node](float t) {
        Variables variables;
        variables[node->parameter] = t;
        return evaluateNumber(node->function.get(), &variables);
    };
}

void Executor::executeFadeAction(FadeActionNode* node) {
    if (!node) {
        return;
    }

    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    timeline->seek(currentTime);

    float duration = node->duration ? evaluateDuration(node->duration.get()) : 0.0f;
    float startOpacity = node->isFadeIn ? 0.0f : objects[index]->colourMask.w;
    float endOpacity = node->isFadeIn ? 1.0f : 0.0f;

    auto animation = std::make_unique<FadeAnimation>(
        index,
        currentTime,
        duration,
        startOpacity,
        endOpacity
    );
    animation->interpolation = getInterpolation(node->interpolation);
    timeline->addAnimation(std::move(animation));

    pendingEndTime = std::max(pendingEndTime, currentTime + duration);
}

void Executor::executeRotateAction(RotateActionNode* node) {
    if (!node || !node->amount) {
        return;
    }

    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    timeline->seek(currentTime);

    float duration = node->duration ? evaluateDuration(node->duration.get()) : 0.0f;
    glm::vec3 amount = evaluateVector(node->amount.get());
    glm::vec3 startRotation = objects[index]->transform.rotation;
    glm::vec3 endRotation = startRotation + amount;

    auto animation = std::make_unique<RotateAnimation>(
        index,
        currentTime,
        duration,
        startRotation,
        endRotation
    );
    animation->interpolation = getInterpolation(node->interpolation);
    timeline->addAnimation(std::move(animation));

    pendingEndTime = std::max(pendingEndTime, currentTime + duration);
}

void Executor::executeMoveAction(MoveActionNode* node) {
    if (!node || !node->amount) {
        return;
    }

    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    timeline->seek(currentTime);

    float duration = node->duration ? evaluateDuration(node->duration.get()) : 0.0f;
    glm::vec3 amount = evaluateVector(node->amount.get());
    glm::vec3 startPosition = objects[index]->transform.position;
    glm::vec3 endPosition = startPosition + amount;

    auto animation = std::make_unique<MoveAnimation>(
        index,
        currentTime,
        duration,
        startPosition,
        endPosition
    );
    animation->interpolation = getInterpolation(node->interpolation);
    timeline->addAnimation(std::move(animation));

    pendingEndTime = std::max(pendingEndTime, currentTime + duration);
}

void Executor::executeScaleAction(ScaleActionNode* node) {
    if (!node || !node->amount) {
        return;
    }

    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    timeline->seek(currentTime);

    float duration = node->duration ? evaluateDuration(node->duration.get()) : 0.0f;
    glm::vec3 amount = evaluateVector(node->amount.get());
    glm::vec3 startScale = objects[index]->transform.scale;
    glm::vec3 endScale = startScale * amount;

    auto animation = std::make_unique<ScaleAnimation>(
        index,
        currentTime,
        duration,
        startScale,
        endScale
    );
    animation->interpolation = getInterpolation(node->interpolation);
    timeline->addAnimation(std::move(animation));

    pendingEndTime = std::max(pendingEndTime, currentTime + duration);
}

void Executor::executeShowAction(ShowActionNode* node) {
    if (!node) {
        return;
    }

    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    auto animation = std::make_unique<FadeAnimation>(
        index,
        currentTime,
        0.0f,
        0.0f,
        1.0f
    );
    timeline->addAnimation(std::move(animation));
}

void Executor::executeEditAction(EditActionNode* node) {
    if (!node || !node->properties ||
        node->properties->getType() != ASTNodeType::ObjectExpression) {
        return;
    }

    ObjectExpressionNode* properties = static_cast<ObjectExpressionNode*>(node->properties.get());
    int index = getObjectIndex(node->object);

    if (index < 0) {
        return;
    }

    timeline->seek(currentTime);

    float duration = node->duration ? evaluateDuration(node->duration.get()) : 0.0f;
    InterpolationFunction interpolation = getInterpolation(node->interpolation);
    Object* object = objects[index].get();

    for (auto& entry : properties->properties) {
        std::string key = entry.first;
        ExpressionNode* expression = entry.second.get();

        if (key == "position" || key == "rotation" || key == "scale") {
            glm::vec3 startValue = key == "position" ? object->transform.position
                : (key == "rotation" ? object->transform.rotation : object->transform.scale);
            glm::vec3 endValue = evaluateVector(expression);

            auto animation = std::make_unique<PropertyAnimation>(
                index,
                currentTime,
                duration,
                [key, startValue, endValue](Object* target, float progress) {
                    glm::vec3 value = glm::mix(startValue, endValue, progress);

                    if (key == "position") {
                        target->transform.position = value;
                    } else if (key == "rotation") {
                        target->transform.rotation = value;
                    } else {
                        target->transform.scale = value;
                    }
                }
            );
            animation->interpolation = interpolation;
            timeline->addAnimation(std::move(animation));
        } else if (key == "colour" || key == "color") {
            glm::vec4 startValue = object->colourMask;
            glm::vec4 endValue = evaluateColor(expression, startValue);

            auto animation = std::make_unique<PropertyAnimation>(
                index,
                currentTime,
                duration,
                [startValue, endValue](Object* target, float progress) {
                    target->colourMask = glm::mix(startValue, endValue, progress);
                }
            );
            animation->interpolation = interpolation;
            timeline->addAnimation(std::move(animation));
        } else if (EvalValue::isShapeNumberKey(key)) {
            float startValue = EvalValue::getShapeNumber(object, key);
            float endValue = evaluateNumber(expression);

            auto animation = std::make_unique<PropertyAnimation>(
                index,
                currentTime,
                duration,
                [key, startValue, endValue](Object* target, float progress) {
                    EvalValue::setShapeNumber(target, key, glm::mix(startValue, endValue, progress));
                }
            );
            animation->interpolation = interpolation;
            timeline->addAnimation(std::move(animation));
        } else {
            EvalValue::setShapeNumber(object, key, evaluateNumber(expression));
        }
    }

    pendingEndTime = std::max(pendingEndTime, currentTime + duration);
}

void Executor::executeWaitAction(WaitActionNode* node) {
    if (!node) {
        return;
    }

    if (node->duration) {
        currentTime += evaluateDuration(node->duration.get());
    } else {
        currentTime = std::max(currentTime, pendingEndTime);
    }
}

float Executor::evaluateNumber(ExpressionNode* node, Variables* variables) {
    EvalValue::EvalContext context;
    context.variables = variables;
    context.interpolations = &interpolations;

    return static_cast<float>(EvalValue::evaluateExpression(node, context).asNumber());
}

float Executor::evaluateDuration(ExpressionNode* node) {
    return evaluateNumber(node);
}

glm::vec3 Executor::evaluateVector(ExpressionNode* node) {
    if (node && node->getType() == ASTNodeType::ObjectExpression) {
        ObjectExpressionNode* object = static_cast<ObjectExpressionNode*>(node);
        glm::vec3 result = glm::vec3(0.0f);

        for (auto& entry : object->properties) {
            if (entry.first == "x") {
                result.x = evaluateNumber(entry.second.get());
            } else if (entry.first == "y") {
                result.y = evaluateNumber(entry.second.get());
            } else if (entry.first == "z") {
                result.z = evaluateNumber(entry.second.get());
            }
        }

        return result;
    }

    EvalValue::EvalContext context;
    context.interpolations = &interpolations;

    return EvalValue::evaluateExpression(node, context).asVector();
}

glm::vec4 Executor::evaluateColor(ExpressionNode* node, glm::vec4 fallback) {
    if (!node) {
        return fallback;
    }

    EvalValue::EvalContext context;
    context.interpolations = &interpolations;
    EvalValue value = EvalValue::evaluateExpression(node, context);

    if (value.kind == EvalValue::Kind::None || value.kind == EvalValue::Kind::String)
        return fallback;

    return value.asColor();
}

float Executor::getPropertyNumber(ObjectExpressionNode* properties, std::string key, float fallback) {
    if (!properties) {
        return fallback;
    }

    for (auto& entry : properties->properties) {
        if (entry.first == key) {
            return evaluateNumber(entry.second.get());
        }
    }

    return fallback;
}

Executor::InterpolationFunction Executor::getInterpolation(std::string name) {
    auto it = interpolations.find(name);

    if (it != interpolations.end()) {
        return it->second;
    }

    if (name == "linear") {
        return [](float t) { return t; };
    }

    if (name == "easeIn") {
        return [](float t) { return t * t * t; };
    }

    if (name == "easeOut") {
        return [](float t) {
            float u = t - 1.0f;
            return u * u * u + 1.0f;
        };
    }

    if (name == "easeInOut") {
        return [](float t) {
            return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
        };
    }

    return nullptr;
}

std::unique_ptr<Object> Executor::createObject(std::string objectType, ObjectExpressionNode* properties) {
    if (!properties) {
        return nullptr;
    }

    std::unique_ptr<Object> object;

    if (objectType == "CUBE") {
        object = std::make_unique<Cube>(
            getPropertyNumber(properties, "size", 2.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "SQUARE") {
        object = std::make_unique<Square>(
            getPropertyNumber(properties, "size", 2.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "RECTANGLE") {
        object = std::make_unique<Rectangle>(
            getPropertyNumber(properties, "width", 2.0f),
            getPropertyNumber(properties, "height", 2.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "CUBOID") {
        object = std::make_unique<Cuboid>(
            getPropertyNumber(properties, "width", 2.0f),
            getPropertyNumber(properties, "height", 2.0f),
            getPropertyNumber(properties, "depth", 2.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "SPHERE") {
        object = std::make_unique<Sphere>(
            getPropertyNumber(properties, "radius", 1.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "CYLINDER") {
        object = std::make_unique<Cylinder>(
            getPropertyNumber(properties, "radius", 1.0f),
            getPropertyNumber(properties, "height", 2.0f),
            glm::vec3(0.0f)
        );
    } else if (objectType == "ELLIPSE" || objectType == "CIRCLE") {
        float radius = getPropertyNumber(properties, "radius", 1.0f);

        object = std::make_unique<Ellipse>(
            getPropertyNumber(properties, "radiusX", radius),
            getPropertyNumber(properties, "radiusY", radius),
            glm::vec3(0.0f)
        );
    } else {
        return nullptr;
    }

    applyObjectProperties(object.get(), properties);
    return object;
}

void Executor::applyObjectProperties(Object* object, ObjectExpressionNode* properties) {
    if (!object || !properties) {
        return;
    }

    Shapes* shape = dynamic_cast<Shapes*>(object);

    if (shape) {
        shape->fill.enabled = true;
        shape->stroke.enabled = false;
        shape->fill.material.albedo = glm::vec4(1.0f);
        shape->stroke.material.albedo = glm::vec4(1.0f);
    }

    for (auto& entry : properties->properties) {
        std::string key = entry.first;
        ExpressionNode* expression = entry.second.get();

        if (key == "position") {
            object->transform.position = evaluateVector(expression);
        } else if (key == "rotation") {
            object->transform.rotation = evaluateVector(expression);
        } else if (key == "scale") {
            object->transform.scale = evaluateVector(expression);
        } else if (key == "colour" || key == "color") {
            object->colourMask = evaluateColor(expression, object->colourMask);
        } else if (key == "fillColour" || key == "fillColor") {
            if (shape) {
                shape->fill.material.albedo = evaluateColor(expression, shape->fill.material.albedo);
            }
        } else if (key == "strokeColour" || key == "strokeColor") {
            if (shape) {
                shape->stroke.material.albedo = evaluateColor(expression, shape->stroke.material.albedo);
            }
        } else {
            EvalValue::setShapeNumber(object, key, evaluateNumber(expression));
        }
    }
}

int Executor::getObjectIndex(std::string name) {
    auto iterator = objectIndices.find(name);

    if (iterator == objectIndices.end()) {
        return -1;
    }

    return iterator->second;
}

void Executor::update(float deltaTime) {
    timeline->update(deltaTime);
}

void Executor::seek(float time) {
    timeline->seek(time);
}

void Executor::reset() {
    currentTime = 0.0f;
    pendingEndTime = 0.0f;

    for (size_t i = 0; i < objects.size() && i < savedStates.size(); i++) {
        restoreObjectState(objects[i].get(), savedStates[i]);
    }

    timeline->reset();
}
