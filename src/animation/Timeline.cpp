
#include "Timeline.h"
#include <algorithm>
#include <utility>

Timeline::Timeline(std::vector<std::unique_ptr<Object>>& objects)
    : objects(objects) {}

void Timeline::addAnimation(std::unique_ptr<Animation> animation) {
    animations.push_back(std::move(animation));
}

void Timeline::clear() {
    animations.clear();
    reset();
}

void Timeline::update(float deltaTime) {
    currentTime += deltaTime;
    seek(currentTime);
}

void Timeline::seek(float time) {
    currentTime = time;
    
    for (const std::unique_ptr<Animation>& animation : animations) {
        if (animation->objectIndex < 0 ||
            animation->objectIndex >= objects.size()) {
            continue;
        }

        Object* object = objects[animation->objectIndex].get();

        if (object == nullptr || time < animation->startTime) {
            continue;
        }

        float progress = animation->duration > 0.0f
            ? std::clamp(
                (time - animation->startTime) / animation->duration,
                0.0f,
                1.0f
            )
            : 1.0f;

        animation->execute(object, progress);
    }
}

void Timeline::reset() {
    currentTime = 0.0f;
    seek(0.0f);
}

