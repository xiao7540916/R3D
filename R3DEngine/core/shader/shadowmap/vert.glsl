#version 460
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUv;
layout (binding = 0) uniform ubobase{
    mat4 viewproj;
};
layout (binding = 1) uniform ubomodel{
    mat4 model;
};
void main() {
    gl_Position = viewproj*model*vec4(vPosition, 1.0);
}
