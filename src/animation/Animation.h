#pragma once

#include <glm/glm.hpp>

enum class AnimationType {
    Move,
    Rotate,
    Fade,
    Draw
};

struct Animation {
    AnimationType type;
    int objectIndex;
    float startTime;
    float duration;

    glm::vec4 startValue;
    glm::vec4 endValue;
};
