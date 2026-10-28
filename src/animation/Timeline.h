#pragma once

#include <vector>
#include "Animation.h"
#include "../scene/Object.h"

class Timeline {
public:
    explicit Timeline(std::vector<Object*>& objects);

    void addAnimation(const Animation& animation);
    void update(float deltaTime);
    void seek(float time);
    void reset();

private:
    std::vector<Object*>& objects;
    std::vector<Animation> animations;
    float currentTime = 0.0f;
};
