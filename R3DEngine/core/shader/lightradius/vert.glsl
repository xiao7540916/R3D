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
    mat4 view;
    mat4 proj;
    mat4 invproj;
    mat4 viewproj;
    vec3 camerapos;
    int dirlightactivenum;//平行光启用数目
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightactivenum;//点光源启用数目
    int tilepointlightmax;//单个块最多点光源数目
    float windowwidth;
    float windowheight;
    int workgroup_x;//用于灯光剔除的横向组数
    float znear;
    float zfar;
    float fill2;
};
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
    vec4 surfacecolor;//透明物体的表面颜色
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
layout(std140, binding = 1) uniform UniformMeshBuffer {
    UniformBlockMesh ubomeshdata;
};
struct PointLight{
    vec3 position;
    float constant;
    vec3 strength;
    float linear;
    float quadratic;
    float radius;
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
    gl_Position = ubobasedata.viewproj*vec4((pointLightData[gl_InstanceID].radius)*vPosition+pointLightData[gl_InstanceID].position, 1.0);
    lightcolor = pointLightData[gl_InstanceID].strength;
    float lightcolormax = max(max(lightcolor.r, lightcolor.g), lightcolor.b);
    if (lightcolormax>1.0f){
        lightcolor = lightcolor/lightcolormax;
    }
}
