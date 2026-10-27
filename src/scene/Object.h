#pragma once

#include <vector>

#include "Transform.h"
#include "Triangle.h"
#include "Material.h"

class Object {
public:
    Transform transform;

    virtual std::vector<Triangle> getTriangles(int materialCount) = 0;
    virtual std::vector<Material> getMaterials() = 0;
};
