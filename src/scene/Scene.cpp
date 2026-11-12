#include "Scene.h"
#include "Shapes.h"

#include <utility>

Scene::Scene() : timeline(objects), renderer() {
    std::unique_ptr<Rectangle> rectangle1 = std::unique_ptr<Rectangle>(new Rectangle(
        6.0f,
        10.0f,
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, glm::radians(45.0f), 0.0f)
    ));
    rectangle1->fill.enabled = true;
    rectangle1->fill.material.albedo = glm::vec4(1.0f, 0.2f, 0.2f, 0.5f);
    rectangle1->stroke.enabled = true;
    rectangle1->stroke.material.albedo = glm::vec4(1.0f);
    rectangle1->stroke.width = 0.1f;
    addObject(std::move(rectangle1));

    std::unique_ptr<Rectangle> rectangle2 = std::unique_ptr<Rectangle>(new Rectangle(
        4.0f,
        4.0f,
        glm::vec3(10.0f, 5.0f, 0.0f),
        glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    ));
    rectangle2->fill.enabled = false;
    rectangle2->stroke.enabled = true;
    rectangle2->stroke.material.albedo = glm::vec4(0.2f, 1.0f, 0.4f, 1.0f);
    rectangle2->stroke.width = 1.0f;
    addObject(std::move(rectangle2));

    std::unique_ptr<Ellipse> ellipse1 = std::unique_ptr<Ellipse>(new Ellipse(
        2.0f,
        1.5f,
        glm::vec3(10.0f, 10.0f, 0.0f),
        32,
        glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    ));
    ellipse1->fill.enabled = false;
    ellipse1->fill.material.albedo = glm::vec4(0.2f, 0.5f, 1.0f, 1.0f);
    ellipse1->stroke.enabled = true;
    ellipse1->stroke.material.albedo = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f);
    ellipse1->stroke.width = 1.0f;
    addObject(std::move(ellipse1));

    std::unique_ptr<Ellipse> ellipse2 = std::unique_ptr<Ellipse>(new Ellipse(
        3.0f,
        1.0f,
        glm::vec3(10.0f, -5.0f, 0.0f),
        32,
        glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    ));
    ellipse2->fill.enabled = false;
    ellipse2->stroke.enabled = true;
    ellipse2->stroke.material.albedo = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);
    ellipse2->stroke.width = 0.15f;
    addObject(std::move(ellipse2));
    
    timeline.addAnimation(std::unique_ptr<MoveAnimation>(new MoveAnimation(
        0,
        1.0f,
        1.0f,
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 5.0f, 0.0f)
    )));
    timeline.addAnimation(std::unique_ptr<DrawAnimation>(new DrawAnimation(3, 0.0f, 1.0f)));
}

void Scene::update(float deltaTime) {
    timeline.update(deltaTime);
    
    triangles = {};
    materials = {};

    for (const std::unique_ptr<Object>& object : objects) {
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

void Scene::addObject(std::unique_ptr<Object> object) {
    objects.push_back(std::move(object));
}
