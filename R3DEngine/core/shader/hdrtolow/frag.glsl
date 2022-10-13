#version 460
layout(location = 0)out vec4 FragColor;
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
    int csmlayercount;
    mat4 lightviewprojdata[6];
    float depthbias;
    float normalbias;
    float hdrexp;
    float fill1;
    vec3 ambient;
    float fill2;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
layout(binding = 0)uniform sampler2D screenTex;
in VS_OUT{
    vec2 uv;
}fs_in;
void main() {
    vec3 srccol = texture(screenTex, fs_in.uv).rgb;
    vec3 dstcol = vec3(1.0) - exp(-srccol * ubobasedata.hdrexp);
    FragColor = vec4(dstcol, 1.0);
}
