#pragma once

#include <vector>

#include "Object.h"

class Shapes : public Object {
public:
    Fill fill;
    Stroke stroke;

    std::vector<Material> getMaterials() override;
};
