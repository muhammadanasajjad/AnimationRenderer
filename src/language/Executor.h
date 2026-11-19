#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "../animation/Timeline.h"
#include "../scene/Object.h"
#include "ASTNode.h"

class Executor {
    using Objects = std::vector<std::unique_ptr<Object>>;

public:
    Executor(Objects& objects);

    void execute(ProgramNode* program);
    void execute(SceneDeclarationNode* scene);
    void update(float deltaTime);
    void seek(float time);
    void reset();

private:
    using InterpolationFunction = std::function<float(float)>;
    using Variables = std::unordered_map<std::string, float>;

    struct SavedObjectState {
        Transform transform;
        glm::vec4 colourMask = glm::vec4(1.0f);
        float sideLength = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float depth = 0.0f;
        float radiusX = 0.0f;
        float radiusY = 0.0f;
        float strokeWidth = 0.0f;
        bool fillEnabled = false;
        bool strokeEnabled = false;
    };

    Objects& objects;
    std::vector<SavedObjectState> savedStates;
    std::unordered_map<std::string, int> objectIndices;
    std::unordered_map<std::string, InterpolationFunction> interpolations;
    std::unique_ptr<Timeline> timeline;

    float currentTime = 0.0f;
    float pendingEndTime = 0.0f;

    void executeObjectDeclaration(ObjectDeclarationNode* node);
    void executeFadeAction(FadeActionNode* node);
    void executeRotateAction(RotateActionNode* node);
    void executeMoveAction(MoveActionNode* node);
    void executeScaleAction(ScaleActionNode* node);
    void executeShowAction(ShowActionNode* node);
    void executeEditAction(EditActionNode* node);
    void executeWaitAction(WaitActionNode* node);
    void executeInterpolationDeclaration(InterpolationDeclarationNode* node);

    float evaluateNumber(ExpressionNode* node, Variables* variables = nullptr);
    float evaluateDuration(ExpressionNode* node);
    glm::vec3 evaluateVector(ExpressionNode* node);
    glm::vec4 evaluateColor(ExpressionNode* node, glm::vec4 fallback);
    static SavedObjectState captureObjectState(Object* object);
    static void restoreObjectState(Object* object, SavedObjectState& state);
    float getPropertyNumber(ObjectExpressionNode* properties, std::string key, float fallback);
    InterpolationFunction getInterpolation(std::string name);
    std::unique_ptr<Object> createObject(std::string objectType, ObjectExpressionNode* properties);
    void applyObjectProperties(Object* object, ObjectExpressionNode* properties);

    int getObjectIndex(std::string name);
};
