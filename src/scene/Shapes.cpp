#include "Shapes.h"

std::vector<Material> Shapes::getMaterials() {
    std::vector<Material> materials;

    if (fill.enabled) {
        Material material = fill.material;
        material.albedo *= colourMask;
        materials.push_back(material);
    }

    if (stroke.enabled) {
        Material material = stroke.material;
        material.albedo *= colourMask;
        materials.push_back(material);
    }

    return materials;
}
