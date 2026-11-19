#version 430 core

struct Triangle {
    vec4 a;
    vec4 b;
    vec4 c;
};

layout(std430, binding = 0) readonly buffer TriangleBuffer {
    Triangle triangles[];
};

uniform int projectedTriangleCount;
uniform int tileRows;
uniform int tileColumns;

layout(std430, binding = 2) buffer TileCount {
    int tileCounts[];
};

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

void triangleTileBounds(vec2 p1, vec2 p2, vec2 p3, out ivec2 minTile, out ivec2 maxTile) {
    float columns = float(tileColumns);
    float rows = float(tileRows);

    vec2 triMin = min(p1, min(p2, p3));
    vec2 triMax = max(p1, max(p2, p3));

    minTile = ivec2(max(0, int(ceil(triMin.x * columns - 1.0))),
                    max(0, int(ceil(triMin.y * rows - 1.0))));
    maxTile = ivec2(min(tileColumns - 1, int(floor(triMax.x * columns))),
                    min(tileRows - 1, int(floor(triMax.y * rows))));
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= projectedTriangleCount)
        return;

    vec2 p1 = triangles[idx].a.xy;
    vec2 p2 = triangles[idx].b.xy;
    vec2 p3 = triangles[idx].c.xy;

    ivec2 minTile;
    ivec2 maxTile;
    triangleTileBounds(p1, p2, p3, minTile, maxTile);

    if (maxTile.x < minTile.x || maxTile.y < minTile.y)
        return;

    for (int ty = minTile.y; ty <= maxTile.y; ty++) {
        int rowStart = ty * tileColumns;
        for (int tx = minTile.x; tx <= maxTile.x; tx++) {
            atomicAdd(tileCounts[rowStart + tx], 1);
        }
    }
}
