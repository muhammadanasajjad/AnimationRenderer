#include "Shapes3D.h"

void Shapes3D::addTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    mesh.push_back({
        glm::vec4(a, 0.0f),
        glm::vec4(b, 0.0f),
        glm::vec4(c, 0.0f)
    });
}

std::vector<Triangle> Shapes3D::getTriangles(int materialCount) {
    std::vector<Triangle> triangles;

    if (!fill.enabled) {
        return triangles;
    }

    for (const Triangle& triangle : mesh) {
        triangles.push_back({
            glm::vec4(transform.apply(glm::vec3(triangle.a)), materialCount),
            glm::vec4(transform.apply(glm::vec3(triangle.b)), 0.0f),
            glm::vec4(transform.apply(glm::vec3(triangle.c)), 0.0f)
        });
    }

    return triangles;
}

void Shapes3D::animateDraw(float) {
}

Cuboid::Cuboid(float width, float height, float depth, glm::vec3 position, glm::vec3 rotation)
    : width(width), height(height), depth(depth) {
    transform.position = position;
    transform.rotation = rotation;

    rebuildMesh();
}

void Cuboid::rebuildMesh() {
    mesh.clear();

    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    glm::vec3 point[8] = {
        {-halfWidth, -halfHeight, -halfDepth},
        { halfWidth, -halfHeight, -halfDepth},
        { halfWidth,  halfHeight, -halfDepth},
        {-halfWidth,  halfHeight, -halfDepth},
        {-halfWidth, -halfHeight,  halfDepth},
        { halfWidth, -halfHeight,  halfDepth},
        { halfWidth,  halfHeight,  halfDepth},
        {-halfWidth,  halfHeight,  halfDepth}
    };

    addTriangle(point[0], point[1], point[2]);
    addTriangle(point[0], point[2], point[3]);
    addTriangle(point[4], point[6], point[5]);
    addTriangle(point[4], point[7], point[6]);
    addTriangle(point[0], point[4], point[5]);
    addTriangle(point[0], point[5], point[1]);
    addTriangle(point[3], point[2], point[6]);
    addTriangle(point[3], point[6], point[7]);
    addTriangle(point[0], point[3], point[7]);
    addTriangle(point[0], point[7], point[4]);
    addTriangle(point[1], point[5], point[6]);
    addTriangle(point[1], point[6], point[2]);
}

Cube::Cube(float sideLength, glm::vec3 position, glm::vec3 rotation)
    : Cuboid(sideLength, sideLength, sideLength, position, rotation), sideLength(sideLength) {
    rebuildMesh();
}

void Cube::rebuildMesh() {
    width = height = depth = sideLength;
    Cuboid::rebuildMesh();
}

Cylinder::Cylinder(float radius, float height, glm::vec3 position, int segments, glm::vec3 rotation)
    : radius(radius), height(height), segments(segments) {
    transform.position = position;
    transform.rotation = rotation;

    rebuildMesh();
}

void Cylinder::rebuildMesh() {
    mesh.clear();

    if (segments < 3) {
        return;
    }

    float halfHeight = height / 2.0f;

    for (int i = 0; i < segments; i++) {
        float firstAngle = 2.0f * glm::pi<float>() * i / segments;
        float secondAngle = 2.0f * glm::pi<float>() * (i + 1) / segments;

        glm::vec3 bottomFirst(radius * glm::cos(firstAngle), -halfHeight, radius * glm::sin(firstAngle));
        glm::vec3 bottomSecond(radius * glm::cos(secondAngle), -halfHeight, radius * glm::sin(secondAngle));
        glm::vec3 topFirst(radius * glm::cos(firstAngle), halfHeight, radius * glm::sin(firstAngle));
        glm::vec3 topSecond(radius * glm::cos(secondAngle), halfHeight, radius * glm::sin(secondAngle));

        addTriangle(bottomFirst, bottomSecond, topSecond);
        addTriangle(bottomFirst, topSecond, topFirst);
        addTriangle(glm::vec3(0.0f, halfHeight, 0.0f), topFirst, topSecond);
        addTriangle(glm::vec3(0.0f, -halfHeight, 0.0f), bottomSecond, bottomFirst);
    }
}

Sphere::Sphere(float radius, glm::vec3 position, int segments, int rings, glm::vec3 rotation)
    : radius(radius), segments(segments), rings(rings) {
    transform.position = position;
    transform.rotation = rotation;

    rebuildMesh();
}

void Sphere::rebuildMesh() {
    mesh.clear();

    if (segments < 3 || rings < 2) {
        return;
    }

    for (int ring = 0; ring < rings; ring++) {
        for (int segment = 0; segment < segments; segment++) {
            float firstPhi = glm::pi<float>() * ring / rings;
            float secondPhi = glm::pi<float>() * (ring + 1) / rings;
            float firstTheta = 2.0f * glm::pi<float>() * segment / segments;
            float secondTheta = 2.0f * glm::pi<float>() * (segment + 1) / segments;

            glm::vec3 firstFirst(
                radius * glm::sin(firstPhi) * glm::cos(firstTheta),
                radius * glm::cos(firstPhi),
                radius * glm::sin(firstPhi) * glm::sin(firstTheta)
            );
            glm::vec3 secondFirst(
                radius * glm::sin(firstPhi) * glm::cos(secondTheta),
                radius * glm::cos(firstPhi),
                radius * glm::sin(firstPhi) * glm::sin(secondTheta)
            );
            glm::vec3 firstSecond(
                radius * glm::sin(secondPhi) * glm::cos(firstTheta),
                radius * glm::cos(secondPhi),
                radius * glm::sin(secondPhi) * glm::sin(firstTheta)
            );
            glm::vec3 secondSecond(
                radius * glm::sin(secondPhi) * glm::cos(secondTheta),
                radius * glm::cos(secondPhi),
                radius * glm::sin(secondPhi) * glm::sin(secondTheta)
            );

            addTriangle(firstFirst, secondFirst, secondSecond);
            addTriangle(firstFirst, secondSecond, firstSecond);
        }
    }
}
