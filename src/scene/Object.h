#pragma once

#include "Transform.h"
#include "Scene.h"

class Object {
public:
    Transform transform;

    virtual void addTriangles(Scene& scene) {};
};

