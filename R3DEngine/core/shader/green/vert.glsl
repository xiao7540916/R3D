#version 460
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUv;
#define DIRECTION_LIGHT_COUNT 4
struct DirLight {
    vec3 direction;
    float fill0;
    vec3 strength;
    float fill1;
};
struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    int dirlightenable;
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightenable;
    int tilepointlightmax;
    float fill1;
    float fill2;
};
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase block;
}ubobasedata;
layout(std140, binding = 1) uniform UniformMeshBuffer {
    UniformBlockMesh block;
}ubomeshdata;

void main() {
    gl_Position = ubobasedata.block.viewproj*ubomeshdata.block.model*vec4(vPosition, 1.0);
}
