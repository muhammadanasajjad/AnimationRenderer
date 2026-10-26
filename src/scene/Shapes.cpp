#include "Shapes.h"
#include "Triangle.h"

std::vector<Triangle> triangulate(std::vector<glm::vec3> outline, int materialIndex) {
    std::vector<Triangle> triangles = {};
    glm::vec4 p1 = {outline[0], materialIndex};
    
    for (int i = 1; i < outline.size()-1; i++) {
        glm::vec4 p2 = {outline[i], 0};
        glm::vec4 p3 = {outline[i+1], 0};
        
        triangles.push_back({p1, p2, p3});
    }
    
    return triangles;
}

void Shape::addTriangles(Scene& scene) {
    std::vector<glm::vec3> transformedOutline;

    for (const auto& point : outline) {
        transformedOutline.push_back(transform.apply(point));
    }

    std::vector<Triangle> triangles = triangulate(transformedOutline, 0);

    for (const auto& triangle : triangles) {
        scene.addTriangle(triangle);
    }
}
