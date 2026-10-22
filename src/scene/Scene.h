#pragma once

#include <vector>

#include "Triangle.h"
#include "../renderer/Renderer.h"

class Scene {
public:
    Scene();
    
    void addTriangle(const Triangle& triangle);
    void render();
    void clearTriangles();

    const std::vector<Triangle>& getTriangles() const;

private:
    std::vector<Triangle> triangles;
    Renderer renderer;
};

