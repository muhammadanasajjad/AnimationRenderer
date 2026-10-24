#pragma once

#include <vector>

#include "Triangle.h"
#include "Material.h"
#include "../renderer/Renderer.h"

class Scene {
public:
    Scene();
    
    void addTriangle(const Triangle& triangle);
    void render();
    void clearTriangles();

private:
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    Renderer renderer;
};

