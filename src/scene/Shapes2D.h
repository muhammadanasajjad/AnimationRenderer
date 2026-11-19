#pragma once

#include <vector>

#include "Shapes.h"

class Shapes2D : public Shapes {
public:
    std::vector<glm::vec3> outline;

    std::vector<Triangle> getTriangles(int materialCount) override;

    void animateDraw(float progress) override;

protected:
    std::vector<glm::vec3> visibleOutline;
    int flag = 0;
};

class Square : public Shapes2D {
public:
    float sideLength;

    Square(float sideLength, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f))
        : sideLength(sideLength) {
        transform.position = position;
        transform.rotation = rotation;

        rebuildOutline();
    }

    void rebuildOutline();
};

class Rectangle : public Shapes2D {
public:
    float width;
    float height;

    Rectangle(float width, float height, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f))
        : width(width), height(height) {
        transform.position = position;
        transform.rotation = rotation;

        rebuildOutline();
    }

    void rebuildOutline();
};

class Ellipse : public Shapes2D {
public:
    float radiusX;
    float radiusY;
    int segments;

    Ellipse(float radiusX, float radiusY, glm::vec3 position, int segments = 32, glm::vec3 rotation = glm::vec3(0.0f))
        : radiusX(radiusX), radiusY(radiusY), segments(segments) {
        transform.position = position;
        transform.rotation = rotation;

        rebuildOutline();
    }

    void rebuildOutline();
};
