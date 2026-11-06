#pragma once

#include <vector>

#include "Transform.h"
#include "Triangle.h"
#include "Material.h"

class Object {
public:
    Transform transform;
    
    glm::vec4 colourMask = glm::vec4(1, 1, 1, 1);

    virtual std::vector<Triangle> getTriangles(int materialCount) = 0;
    virtual std::vector<Material> getMaterials() = 0;
};
