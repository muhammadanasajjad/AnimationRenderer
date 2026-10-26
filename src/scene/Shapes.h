#pragma once

#include <vector>

#include "Object.h"

class Shape : public Object {
public:
    std::vector<glm::vec3> outline;
    // TODO: add stroke and fill styles;
    
    void addTriangles(Scene& scene) override;
private:
    // TODO: use enum
    int flag = 0; // 0 is triangle filled
};

class Square : public Shape {
public:
    float sideLength;

    Square(float sideLength, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f))
        : sideLength(sideLength) {
        transform.position = position;
        transform.rotation = rotation;

        float half = sideLength / 2.0f;

        outline = {
            {-half, -half, 0.0f},
            { half, -half, 0.0f},
            { half,  half, 0.0f},
            {-half,  half, 0.0f}
        };
    }
};
