#pragma once

#include <vector>
#include "Animation.h"

class Timeline {
public:
    void addAnimation(const Animation& animation);
    void update(Scene& scene, float deltaTime);
    void seek(Scene& scene, float time);
    void reset(Scene& scene);

private:
    std::vector<Animation> animations;
    float currentTime = 0.0f;
};
