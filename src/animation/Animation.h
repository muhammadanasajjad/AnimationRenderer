#pragma once

#include <functional>
#include <utility>

#include <glm/glm.hpp>
#include "../scene/Object.h"

enum class AnimationType {
    Move,
    Rotate,
    Scale,
    Fade,
    Draw
};

class Animation {
public:
    int objectIndex;
    float startTime;
    float duration;
    std::function<float(float)> interpolation;

    Animation(int objectIndex, float startTime, float duration)
        : objectIndex(objectIndex), startTime(startTime), duration(duration) {}

    virtual ~Animation() = default;

    float applyInterpolation(float progress) const {
        return interpolation ? interpolation(progress) : progress;
    }

    virtual void execute(Object* object, float progress) = 0;
};

class MoveAnimation : public Animation {
public:
    
    MoveAnimation(int objectIndex, float startTime, float duration, glm::vec3 startPosition, glm::vec3 endPosition)
        : Animation(objectIndex, startTime, duration), startPosition(startPosition), endPosition(endPosition) {}
    
    void execute(Object* object, float progress) override {
        glm::vec3 value = glm::mix(
            startPosition,
            endPosition,
            applyInterpolation(progress)
        );
        
        object->transform.position = value;
    }
    
private:
    glm::vec3 startPosition;
    glm::vec3 endPosition;
};

class RotateAnimation : public Animation {
public:

    RotateAnimation(int objectIndex, float startTime, float duration, glm::vec3 startRotation, glm::vec3 endRotation)
        : Animation(objectIndex, startTime, duration), startRotation(startRotation), endRotation(endRotation) {}

    void execute(Object* object, float progress) override {
        glm::vec3 value = glm::mix(
            startRotation,
            endRotation,
            applyInterpolation(progress)
        );
        
        object->transform.rotation = value;
    }

private:
    glm::vec3 startRotation;
    glm::vec3 endRotation;
};

class ScaleAnimation : public Animation {
public:

    ScaleAnimation(int objectIndex, float startTime, float duration, glm::vec3 startScale, glm::vec3 endScale)
        : Animation(objectIndex, startTime, duration), startScale(startScale), endScale(endScale) {}

    void execute(Object* object, float progress) override {
        glm::vec3 value = glm::mix(
            startScale,
            endScale,
            applyInterpolation(progress)
        );

        object->transform.scale = value;
    }

private:
    glm::vec3 startScale;
    glm::vec3 endScale;
};

class FadeAnimation : public Animation {
public:

    FadeAnimation(int objectIndex, float startTime, float duration, float startOpacity, float endOpacity)
        : Animation(objectIndex, startTime, duration), startOpacity(startOpacity), endOpacity(endOpacity) {}

    void execute(Object* object, float progress) override {
        float value = glm::mix(
            startOpacity,
            endOpacity,
            applyInterpolation(progress)
        );
        
        object->colourMask.w = value;
    }

private:
    float startOpacity;
    float endOpacity;
};


class PropertyAnimation : public Animation {
public:
    PropertyAnimation(int objectIndex, float startTime, float duration, std::function<void(Object*, float)> apply)
        : Animation(objectIndex, startTime, duration), apply(std::move(apply)) {}

    void execute(Object* object, float progress) override {
        if (apply) {
            apply(object, applyInterpolation(progress));
        }
    }

private:
    std::function<void(Object*, float)> apply;
};

class DrawAnimation : public Animation {
public:

    DrawAnimation(int objectIndex, float startTime, float duration)
        : Animation(objectIndex, startTime, duration) {}

    void execute(Object* object, float progress) override {
        object->animateDraw(progress);
    }

private:
    float startOpacity;
    float endOpacity;
};
