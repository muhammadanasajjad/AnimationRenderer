#include "Scene.h"
#include "Shapes.h"

Scene::Scene() : renderer() {
    Square square(4.0f, glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
    square.addTriangles(*this);

    materials = {
        { glm::vec4(1.0, 0.5, 1.0, 1.0) },
        { glm::vec4(0.5, 1.0, 1.0, 1.0) }
    };
}

void Scene::render() {
    renderer.render(triangles, materials);
}

void Scene::addTriangle(const Triangle& triangle) {
    triangles.push_back(triangle);
}

void Scene::clearTriangles() {
    triangles.clear();
}
