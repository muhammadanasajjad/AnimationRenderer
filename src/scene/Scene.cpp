#include "Scene.h"

#include <utility>

Scene::Scene() : timeline(objects), renderer() {
}

void Scene::update(float deltaTime) {
    timeline.update(deltaTime);
    
    triangles = {};
    materials = {};

    for (const std::unique_ptr<Object>& object : objects) {
        if (object->colourMask.a <= 0.0f) {
            continue;
        }

        std::vector<Triangle> objectTriangles = object->getTriangles(materials.size());
        std::vector<Material> objectMaterials = object->getMaterials();

        for (const Triangle& triangle : objectTriangles) {
            triangles.push_back(triangle);
        }

        for (const Material& material : objectMaterials) {
            materials.push_back(material);
        }
    }
}

void Scene::render() {
    renderer.render(triangles, materials);
}

void Scene::addTriangle(const Triangle& triangle) {
    triangles.push_back(triangle);
}

void Scene::addMaterial(const Material& material) {
    materials.push_back(material);
}

int Scene::getMaterialCount() {
    return materials.size();
}

void Scene::clearTriangles() {
    triangles.clear();
}

std::vector<std::unique_ptr<Object>>& Scene::getObjects() {
    return objects;
}

void Scene::addObject(std::unique_ptr<Object> object) {
    objects.push_back(std::move(object));
}
