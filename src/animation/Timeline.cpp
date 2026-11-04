
#include "Timeline.h"
#include <algorithm>

Timeline::Timeline(std::vector<Object*>& objects)
    : objects(objects) {}

void Timeline::addAnimation(Animation* animation) {
    animations.push_back(animation);
}

void Timeline::update(float deltaTime) {
    currentTime += deltaTime;
    seek(currentTime);
}

void Timeline::seek(float time) {
    currentTime = time;
    
    for (Animation* animation : animations) {
        if (animation->objectIndex < 0 ||
            animation->objectIndex >= objects.size()) {
            continue;
        }

        Object* object = objects[animation->objectIndex];

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

