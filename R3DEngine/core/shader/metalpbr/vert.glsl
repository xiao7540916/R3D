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
    int csmlayercount;
    mat4 lightviewprojdata[6];
    float depthbias;
    float normalbias;
    float fill0;
    float fill1;
};
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
};
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
layout(std140, binding = 1) uniform UniformMeshBuffer {
    UniformBlockMesh ubomeshdata;
};

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    float CameraZ;
} vs_out;
void main() {
    gl_Position = ubobasedata.viewproj*ubomeshdata.model*vec4(vPosition, 1.0);
    vs_out.FragPos = vec3(ubomeshdata.model * vec4(vPosition, 1.0));
    vs_out.TexCoords = ubomeshdata.uvoffset+vUv*ubomeshdata.uvscale;
    mat3 normalMatrix = mat3(ubomeshdata.invmodelt);
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
    vs_out.CameraZ = -(ubobasedata.view*vec4(vPosition, 1.0f)).z;
}
