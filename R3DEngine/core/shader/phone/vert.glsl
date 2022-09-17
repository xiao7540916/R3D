#version 460
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUv;
struct UniformBlockBase {
    mat4 model;
    mat4 viewproj;
};
layout(std140, binding = 0) uniform UniformBuffer {
    UniformBlockBase block;
}ubodata;
out vec2 fraguv;
void main() {
    gl_Position = ubodata.block.viewproj*ubodata.block.model*vec4(vPosition, 1.0);
    fraguv = vUv;
}
