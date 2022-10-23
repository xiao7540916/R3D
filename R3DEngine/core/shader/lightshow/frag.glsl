#version 460
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D albedoTex;
layout(binding = 1)uniform sampler2D normalTex;
layout(binding = 2)uniform sampler2D metalTex;
layout(binding = 3)uniform sampler2D roughnessTex;
layout(binding = 4)uniform sampler2D aoTex;

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
    mat4 invviewproj;
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
    int csmlayercount;
    mat4 lightviewprojdata[6];
    float depthbias;
    float normalbias;
    float fill0;
    float fill1;
    vec3 ambient;
    float fill2;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};


const float PI = 3.14159265359;
in vec3 lightcolor;
void main() {
    FragColor = vec4(lightcolor, 1.0);
}
