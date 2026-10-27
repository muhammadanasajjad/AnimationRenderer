#pragma once

#include <vector>

#include "Triangle.h"
#include "Material.h"
#include "Shapes.h"
#include "../renderer/Renderer.h"

class Scene {
public:
    Scene();

    std::vector<Shape*> objects;

    void addObject(Shape* object);
    void addTriangle(const Triangle& triangle);
    void addMaterial(const Material& material);
    int getMaterialCount();
    void render();
    void clearTriangles();

private:
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    Renderer renderer;
};
