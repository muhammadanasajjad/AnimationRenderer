#include "Scene.h"
#include "Shapes.h"

Scene::Scene() : timeline(objects), renderer() {
    Rectangle* rectangle1 = new Rectangle(
        6.0f,
        3.0f,
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, glm::radians(45.0f), 0.0f)
    );
    rectangle1->fill.enabled = true;
    rectangle1->fill.material.albedo = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
    rectangle1->stroke.enabled = true;
    rectangle1->stroke.material.albedo = glm::vec4(1.0f);
    rectangle1->stroke.width = 0.1f;
    addObject(rectangle1);

    Rectangle* rectangle2 = new Rectangle(
        4.0f,
        4.0f,
        glm::vec3(10.0f, 5.0f, 0.0f),
        glm::vec3(0.0f, glm::radians(20.0f), 0.0f)
    );
    rectangle2->fill.enabled = false;
    rectangle2->stroke.enabled = true;
    rectangle2->stroke.material.albedo = glm::vec4(0.2f, 1.0f, 0.4f, 1.0f);
    rectangle2->stroke.width = 1.0f;
    addObject(rectangle2);

    Ellipse* ellipse1 = new Ellipse(
        2.0f,
        1.5f,
        glm::vec3(10.0f, 10.0f, 0.0f),
        32,
        glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    );
    ellipse1->fill.enabled = false;
    ellipse1->fill.material.albedo = glm::vec4(0.2f, 0.5f, 1.0f, 1.0f);
    ellipse1->stroke.enabled = true;
    ellipse1->stroke.material.albedo = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f);
    ellipse1->stroke.width = 1.0f;
    addObject(ellipse1);

    Ellipse* ellipse2 = new Ellipse(
        3.0f,
        1.0f,
        glm::vec3(10.0f, -5.0f, 0.0f),
        32,
        glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    );
    ellipse2->fill.enabled = false;
    ellipse2->stroke.enabled = true;
    ellipse2->stroke.material.albedo = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);
    ellipse2->stroke.width = 0.15f;
    addObject(ellipse2);
    
    timeline.addAnimation({
        AnimationType::Move,
        0,
        0.0f,
        2.0f,
        glm::vec4(10.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(10.0f, 5.0f, 0.0f, 0.0f)
    });
}

void Scene::update(float deltaTime) {
    timeline.update(deltaTime);
    
    triangles = {};
    materials = {};

    for (Object* object : objects) {
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

void Scene::addObject(Object* object) {
    objects.push_back(object);
}
