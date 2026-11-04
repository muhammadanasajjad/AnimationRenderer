#pragma once

#include <glm/glm.hpp>
#include "../scene/Object.h"

enum class AnimationType {
    Move,
    Rotate,
    Fade,
    Draw
};

// TODO: add interpolation methods
class Animation {
public:
    int objectIndex;
    float startTime;
    float duration;
    
    Animation(int objectIndex, float startTime, float duration)
        : objectIndex(objectIndex), startTime(startTime), duration(duration) {}
    
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
            progress
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
            progress
        );
        
        object->transform.rotation = value;
    }

private:
    glm::vec3 startRotation;
    glm::vec3 endRotation;
};
