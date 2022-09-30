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
    int dirlightactivenum;
    DirLight dirLights[DIRECTION_LIGHT_COUNT];
    int pointlightactivenum;
    int tilepointlightmax;
    float windowwidth;
    float windowheight;
    int workgroup_x;
    float fill0;
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

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;
void main() {
    gl_Position = ubobasedata.block.viewproj*ubomeshdata.block.model*vec4(vPosition, 1.0);
    vs_out.FragPos = vec3(ubomeshdata.block.model * vec4(vPosition, 1.0));
    vs_out.TexCoords = ubomeshdata.block.uvoffset+vUv*ubomeshdata.block.uvscale;
    mat3 normalMatrix = mat3(ubomeshdata.block.invmodelt);
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);
}
