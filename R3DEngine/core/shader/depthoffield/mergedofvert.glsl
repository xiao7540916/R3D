#version 460
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUv;
out vec2 uv;
void main() {
    gl_Position = vec4(vPosition, 1.0);
    uv = vUv;
}