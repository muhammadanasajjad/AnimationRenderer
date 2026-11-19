#include "Shapes.h"

bool Shapes::hasVisibleFill() {
    return fill.enabled && fill.material.albedo.a * colourMask.a > 0.0f;
}

bool Shapes::hasVisibleStroke() {
    return stroke.enabled && stroke.material.albedo.a * colourMask.a > 0.0f;
}

std::vector<Material> Shapes::getMaterials() {
    std::vector<Material> materials;

    if (hasVisibleFill()) {
        Material material = fill.material;
        material.albedo *= colourMask;
        materials.push_back(material);
    }

    if (hasVisibleStroke()) {
        Material material = stroke.material;
        material.albedo *= colourMask;
        materials.push_back(material);
    }

    return materials;
}
