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
    float fill0;
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
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
struct PointLight{
    vec3 position;
    float constant;
    vec3 strength;
    float linear;
    float quadratic;
    float fill0;
    float fill1;
    float fill2;
};
#define POINT_LIGHT_COUNT 1024
#define TILE_LIGHT_MAX 128
layout (std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLightData[POINT_LIGHT_COUNT];
};
out vec3 lightcolor;
void main() {
    gl_Position = ubobasedata.block.viewproj*vec4(vPosition+pointLightData[gl_InstanceID].position, 1.0);
    lightcolor = pointLightData[gl_InstanceID].strength;
}
