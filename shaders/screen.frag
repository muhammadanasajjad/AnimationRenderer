#version 430 core

in vec2 uv;
in vec3 cameraPosition;
in vec3 cameraUp;
in vec3 cameraForward;
in vec3 cameraRight;

out vec4 fragColor;

vec2 projectPoint(vec3 point) {
    vec3 pointVector = point - cameraPosition;
    
    float a = abs(dot(pointVector, cameraForward));
    float b = length(pointVector);
    
    pointVector *= (1/a);
    
    float projectedX = -dot(cameraRight, pointVector) * (1/length(cameraRight)); // remove divide by length as length should be 1 as vectors should be normalised when input
    float projectedY = dot(cameraUp, pointVector) * (1/length(cameraUp)); // same as above as these division are expensive
    
    vec2 projected = vec2((1+projectedX)/2, (1+projectedY)/2); // possible remove the changing to uv coordinates instead change the uv coordinates into these coordinates for each pixel
    return projected;
}

bool pointInTriangle(vec2 point, vec2 triP1, vec2 triP2, vec2 triP3) {
    vec2 line12 = triP2 - triP1;
    vec2 line23 = triP3 - triP2;
    vec2 line31 = triP1 - triP3;
    
    vec2 rotated12 = vec2(-line12.y, line12.x);
    vec2 rotated23 = vec2(-line23.y, line23.x);
    vec2 rotated31 = vec2(-line31.y, line31.x);
    
    vec2 toPixel = uv - triP1;
    float dot1 = dot(toPixel, rotated12);
    toPixel = uv - triP2;
    float dot2 = dot(toPixel, rotated23);
    toPixel = uv - triP3;
    float dot3 = dot(toPixel, rotated31);
    
    return (dot1 < 0 && dot2 < 0 && dot3 < 0) || (dot1 > 0 && dot2 > 0 && dot3 > 0);
}

void main() {
    fragColor = vec4(uv, 0.0, 1.0);
    
    vec3 point1 = vec3(10, 5, 5);
    vec3 point2 = vec3(15, -5, 5);
    vec3 point3 = vec3(15, 5, 5);

    vec2 projected1 = projectPoint(point1);
    vec2 projected2 = projectPoint(point2);
    vec2 projected3 = projectPoint(point3);
    
    fragColor = vec4(0);
    bool inside = pointInTriangle(uv, projected1, projected2, projected3);
    if (inside) fragColor = vec4(1);
}

