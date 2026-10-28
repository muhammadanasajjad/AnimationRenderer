#pragma once

#include <vector>

#include "Triangle.h"
#include "Material.h"
#include "Object.h"
#include "../animation/Timeline.h"
#include "../renderer/Renderer.h"

class Scene {
public:
    Scene();

    void addObject(Object* object);
    void addTriangle(const Triangle& triangle);
    void addMaterial(const Material& material);
    int getMaterialCount();
    void update(float deltaTime);
    void render();
    void clearTriangles();

private:
    std::vector<Object*> objects;
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    Renderer renderer;
    Timeline timeline;
};
