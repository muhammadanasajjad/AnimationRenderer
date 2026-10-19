#version 430 core

layout (location = 0) in vec3 position;

out vec2 uv;
out vec3 cameraForward;
out vec3 cameraUp;
out vec3 cameraRight;

void main() {
    gl_Position = vec4(position, 1.0);
    
    uv = vec2((1+position.x)/2, (1+position.y)/2);
    
    cameraForward = vec3(1,0,0);
    cameraUp = vec3(0,1,0);
    cameraRight = cross(cameraUp, cameraForward);
}
