#pragma once

#include <glm/glm.hpp>

struct Material {
    glm::vec4 albedo = glm::vec4(1.0f);
};

struct Fill {
    bool enabled = true;
    Material material;
};

struct Stroke {
    bool enabled = false;
    Material material;
    float width = 1.0f;
};
