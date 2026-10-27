#pragma once

#include <vector>

#include "Object.h"

class Shape : public Object {
public:
    std::vector<glm::vec3> outline;
    Fill fill;
    Stroke stroke;
    
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

class Rectangle : public Shape {
public:
    float width;
    float height;

    Rectangle(float width, float height, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f))
        : width(width), height(height) {
        transform.position = position;
        transform.rotation = rotation;

        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;

        outline = {
            {-halfWidth, -halfHeight, 0.0f},
            { halfWidth, -halfHeight, 0.0f},
            { halfWidth,  halfHeight, 0.0f},
            {-halfWidth,  halfHeight, 0.0f}
        };
    }
};

class Ellipse : public Shape {
public:
    float radiusX;
    float radiusY;
    int segments;

    Ellipse(float radiusX, float radiusY, glm::vec3 position, int segments = 32, glm::vec3 rotation = glm::vec3(0.0f))
        : radiusX(radiusX), radiusY(radiusY), segments(segments) {
        transform.position = position;
        transform.rotation = rotation;

        for (int i = 0; i < segments; i++) {
            float angle = 2.0f * glm::pi<float>() * i / segments;

            outline.push_back({
                radiusX * cos(angle),
                radiusY * sin(angle),
                0.0f
            });
        }
    }
};
