#version 460
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUv;
struct UniformBlockBase {
    mat4 viewproj;
    vec3 camerapos;
    float fill0;
};
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
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
    vs_out.TexCoords = vUv;
    mat3 normalMatrix = mat3(ubomeshdata.block.invmodelt);
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);
}
