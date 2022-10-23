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
layout(binding = 0)uniform sampler2D screenTex;
in VS_OUT{
    vec2 uv;
}fs_in;
void main() {
    vec3 srccol = texture(screenTex, fs_in.uv).rgb;
    //    vec3 dstcol = vec3(1.0) - exp(-srccol * ubobasedata.hdrexp);
    //使用稳定的色调映射变换到低动态范围FXAA后再反变换，最终变换使用对数变换
    vec3 dstcol = srccol / (srccol + vec3(1.0));
    dstcol = pow(dstcol, vec3(0.45));
    FragColor = vec4(dstcol, 1.0);
}
