#include "Scene.h"

Scene::Scene() : renderer() {
    triangles = {
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 0), glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f) },
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f) },

        { glm::vec4(10.0f,     2.0f,  2.8284f, 0), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },
        { glm::vec4(10.0f,     2.0f,  2.8284f, 1), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },

        { glm::vec4(10.0f,    -2.0f,  2.8284f, 0), glm::vec4(12.8284f, -2.0f,  0.0f,    1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f) },
        { glm::vec4(10.0f,    -2.0f,  2.8284f, 1), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f) },

        { glm::vec4(12.8284f, -2.0f,  0.0f,    0), glm::vec4(10.0f,    -2.0f, -2.8284f, 1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },
        { glm::vec4(12.8284f, -2.0f,  0.0f,    1), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f), glm::vec4(12.8284f,  2.0f,  0.0f,    1.0f) },

        { glm::vec4(10.0f,    -2.0f, -2.8284f, 0), glm::vec4(7.1716f,  -2.0f,  0.0f,    1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },
        { glm::vec4(10.0f,    -2.0f, -2.8284f, 1), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f), glm::vec4(10.0f,     2.0f, -2.8284f, 1.0f) },

        { glm::vec4(7.1716f,  -2.0f,  0.0f,    0), glm::vec4(10.0f,    -2.0f,  2.8284f, 1.0f), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f) },
        { glm::vec4(7.1716f,  -2.0f,  0.0f,    1), glm::vec4(10.0f,     2.0f,  2.8284f, 1.0f), glm::vec4(7.1716f,   2.0f,  0.0f,    1.0f) },
    }; // TODO: this is temporary
    
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
