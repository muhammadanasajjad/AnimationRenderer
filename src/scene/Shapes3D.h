#pragma once

#include <vector>

#include "Shapes.h"

class Shapes3D : public Shapes {
public:
    std::vector<Triangle> getTriangles(int materialCount) override;
    void animateDraw(float progress) override;

    virtual void rebuildMesh() = 0;

protected:
    std::vector<Triangle> mesh;

    void addTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
};

class Cuboid : public Shapes3D {
public:
    float width;
    float height;
    float depth;

    Cuboid(float width, float height, float depth, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f));

    void rebuildMesh() override;
};

class Cube : public Cuboid {
public:
    float sideLength;

    Cube(float sideLength, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f));

    void rebuildMesh() override;
};

class Cylinder : public Shapes3D {
public:
    float radius;
    float height;
    int segments;

    Cylinder(float radius, float height, glm::vec3 position, int segments = 32, glm::vec3 rotation = glm::vec3(0.0f));

    void rebuildMesh() override;
};

class Sphere : public Shapes3D {
public:
    float radius;
    int segments;
    int rings;

    Sphere(float radius, glm::vec3 position, int segments = 32, int rings = 16, glm::vec3 rotation = glm::vec3(0.0f));

    void rebuildMesh() override;
};
