#include "Shapes.h"
#include "Triangle.h"

std::vector<Triangle> triangulate(std::vector<glm::vec3> outline, int materialIndex, int flag) {
    std::vector<Triangle> triangles = {};
    glm::vec4 p1 = {outline[0], materialIndex};

    for (int i = 1; i < outline.size() - 1; i++) {
        glm::vec4 p2 = {outline[i], flag};
        glm::vec4 p3 = {outline[i + 1], 0};

        triangles.push_back({p1, p2, p3});
    }

    return triangles;
}

std::vector<Triangle> strokeOutline(const std::vector<glm::vec3>& outline, float width, int materialIndex) {
    std::vector<Triangle> triangles = {};
    size_t outlineSize = outline.size();
    if (outlineSize < 3) return triangles;

    glm::vec3 normal(0.0f);

    for (int i = 0; i < outlineSize; i++) {
        glm::vec3 current = outline[i];
        glm::vec3 next = outline[(i + 1) % outlineSize];

        normal.x += (current.y - next.y) * (current.z + next.z);
        normal.y += (current.z - next.z) * (current.x + next.x);
        normal.z += (current.x - next.x) * (current.y + next.y);
    }

    if (glm::length(normal) < 1e-6f) return triangles;
    normal = glm::normalize(normal);

    float halfWidth = width / 2.0f;
    float maximumOffsetScale = 4.0f;

    std::vector<glm::vec3> outerPoints(outlineSize);
    std::vector<glm::vec3> innerPoints(outlineSize);

    for (int i = 0; i < outlineSize; i++) {
        glm::vec3 previous = outline[(i + outlineSize - 1) % outlineSize];
        glm::vec3 current = outline[i];
        glm::vec3 next = outline[(i + 1) % outlineSize];

        glm::vec3 previousDirection = current - previous;
        glm::vec3 nextDirection = next - current;

        float previousLength = glm::length(previousDirection);
        float nextLength = glm::length(nextDirection);

        if (previousLength < 1e-6f && nextLength < 1e-6f) {
            outerPoints[i] = innerPoints[i] = current;
            continue;
        }

        previousDirection = previousLength < 1e-6f ? nextDirection / nextLength : previousDirection / previousLength;
        nextDirection = nextLength < 1e-6f ? previousDirection : nextDirection / nextLength;

        glm::vec3 previousPerpendicular = glm::cross(normal, previousDirection);
        glm::vec3 nextPerpendicular = glm::cross(normal, nextDirection);

        glm::vec3 offsetDirection = previousPerpendicular + nextPerpendicular;
        glm::vec3 offset;

        if (glm::length(offsetDirection) < 1e-6f) {
            offset = previousPerpendicular * halfWidth;
        } else {
            offsetDirection = glm::normalize(offsetDirection);

            float offsetScale = 1.0f / glm::max(glm::dot(offsetDirection, previousPerpendicular), 1e-3f);
            offsetScale = glm::min(offsetScale, maximumOffsetScale);

            offset = offsetDirection * halfWidth * offsetScale;
        }

        outerPoints[i] = current + offset;
        innerPoints[i] = current - offset;
    }

    for (int i = 0; i < outlineSize; i++) {
        int next = (i + 1) % outlineSize;

        glm::vec4 outerCurrent(outerPoints[i], materialIndex);
        glm::vec4 innerCurrent(innerPoints[i], 0);
        glm::vec4 outerNext(outerPoints[next], 0);
        glm::vec4 innerNext(innerPoints[next], 0);

        triangles.push_back({outerCurrent, innerCurrent, outerNext});
        triangles.push_back({glm::vec4(innerPoints[i], materialIndex), innerNext, glm::vec4(outerPoints[next], 0)});
    }

    return triangles;
}

void Shape::addTriangles(Scene& scene) {
    std::vector<glm::vec3> transformedOutline;

    for (const auto& point : outline) {
        transformedOutline.push_back(transform.apply(point));
    }

    if (fill.enabled) {
        std::vector<Triangle> triangles = triangulate(
            transformedOutline,
            scene.getMaterialCount(),
            flag
        );

        scene.addMaterial(fill.material);

        for (const auto& triangle : triangles) {
            scene.addTriangle(triangle);
        }
    }

    if (stroke.enabled) {
        std::vector<Triangle> triangles = strokeOutline(
            transformedOutline,
            stroke.width,
            scene.getMaterialCount()
        );

        scene.addMaterial(stroke.material);

        for (const auto& triangle : triangles) {
            scene.addTriangle(triangle);
        }
    }
}
