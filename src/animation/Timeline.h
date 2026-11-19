#pragma once

#include <memory>
#include <vector>
#include "Animation.h"
#include "../scene/Object.h"

class Timeline {
public:
    explicit Timeline(std::vector<std::unique_ptr<Object>>& objects);

    void addAnimation(std::unique_ptr<Animation> animation);
    void clear();
    void update(float deltaTime);
    void seek(float time);
    void reset();

private:
    std::vector<std::unique_ptr<Object>>& objects;
    std::vector<std::unique_ptr<Animation>> animations;
    float currentTime = 0.0f;
};
