#version 430 core

struct Triangle {
    vec4 a;
    vec4 b;
    vec4 c;
};

struct Material {
    vec4 albedo;
};

layout(std430, binding = 0) buffer TriangleBuffer {
    Triangle triangles[];
};

layout(std430, binding = 1) buffer MaterialBuffer {
    Material materials[];
};

uniform uint triangleCount;

in vec2 uv;

out vec4 fragColor;

bool pointInTriangle(vec2 point, vec2 triP1, vec2 triP2, vec2 triP3) {
    vec2 line12 = triP2 - triP1;
    vec2 line23 = triP3 - triP2;
    vec2 line31 = triP1 - triP3;
    
    vec2 rotated12 = vec2(-line12.y, line12.x);
    vec2 rotated23 = vec2(-line23.y, line23.x);
    vec2 rotated31 = vec2(-line31.y, line31.x);
    
    vec2 toPixel = point - triP1;
    float dot1 = dot(toPixel, rotated12);
    toPixel = point - triP2;
    float dot2 = dot(toPixel, rotated23);
    toPixel = point - triP3;
    float dot3 = dot(toPixel, rotated31);
    
    return (dot1 < 0 && dot2 < 0 && dot3 < 0) || (dot1 > 0 && dot2 > 0 && dot3 > 0);
}

void main() {
    fragColor = vec4(0);

    vec4 albedos[5];
    float depths[5];
    int depthCount = 0;

    for (int i = 0; i < triangleCount; i++) {
        Triangle tri = triangles[i];

        vec2 projected1 = tri.a.xy;
        vec2 projected2 = tri.b.xy;
        vec2 projected3 = tri.c.xy;

        bool inside = pointInTriangle(uv, projected1, projected2, projected3);

        if (!inside) continue;

        float alpha = cross(vec3(projected2 - projected3, 0), vec3(uv - projected3, 0)).z;
        float beta  = cross(vec3(projected3 - projected1, 0), vec3(uv - projected1, 0)).z;
        float gamma = cross(vec3(projected1 - projected2, 0), vec3(uv - projected2, 0)).z;

        float totalArea = cross(vec3(projected2 - projected1, 0), vec3(projected3 - projected1, 0)).z;
        float areaReciprocal = -1.0 / totalArea;

        alpha *= areaReciprocal;
        beta  *= areaReciprocal;
        gamma *= areaReciprocal;

        float depth = 1.0 / (
            alpha / tri.a.z +
            beta / tri.b.z +
            gamma / tri.c.z
        );

        int insertIndex = depthCount;

        for (int j = 0; j < depthCount; j++) {
            if (depth < depths[j]) {
                insertIndex = j;
                break;
            }
        }

        if (insertIndex >= 5) continue;

        int newCount = min(depthCount + 1, 5);

        for (int j = newCount - 1; j > insertIndex; j--) {
            depths[j] = depths[j - 1];
            albedos[j] = albedos[j - 1];
        }

        depths[insertIndex] = depth;
        albedos[insertIndex] = materials[int(tri.a.a)].albedo;

        depthCount = newCount;
    }

    for (int i = depthCount - 1; i >= 0; i--) {
        vec4 albedo = albedos[i];

        fragColor.rgb = mix(fragColor.rgb, albedo.rgb, albedo.a);
        fragColor.a = albedo.a + fragColor.a * (1.0 - albedo.a);
    }
}
