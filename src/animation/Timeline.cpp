
#include "Timeline.h"
#include <algorithm>

Timeline::Timeline(std::vector<Object*>& objects)
    : objects(objects) {}

void Timeline::addAnimation(const Animation& animation) {
    animations.push_back(animation);
}

void Timeline::update(float deltaTime) {
    currentTime += deltaTime;
    seek(currentTime);
}

void Timeline::seek(float time) {
    currentTime = time;

    for (const Animation& animation : animations) {
        if (animation.objectIndex < 0 ||
            animation.objectIndex >= objects.size()) {
            continue;
        }

        Object* object = objects[animation.objectIndex];

        if (object == nullptr || time < animation.startTime) {
            continue;
        }

        float progress = animation.duration > 0.0f
            ? std::clamp(
                (time - animation.startTime) / animation.duration,
                0.0f,
                1.0f
            )
            : 1.0f;

        glm::vec4 value = glm::mix(
            animation.startValue,
            animation.endValue,
            progress
        );

        if (animation.type == AnimationType::Move) {
            object->transform.position = glm::vec3(value);
        } else if (animation.type == AnimationType::Rotate) {
            object->transform.rotation = glm::vec3(value);
        } else if (animation.type == AnimationType::Fade) {
            // TODO: add transparency
        } else if (animation.type == AnimationType::Draw) {
            // TODO: edit outline
        }
    }
}

void Timeline::reset() {
    currentTime = 0.0f;
    seek(0.0f);
}

