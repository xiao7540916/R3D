#version 460
#extension GL_ARB_bindless_texture : require
layout (early_fragment_tests) in;
#define DIRECTION_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_SIZE 16
#define TILE_LIGHT_MAX 256
struct UniformBlockMesh{
    mat4 model;
    mat4 invmodelt;
    vec2 uvoffset;
    vec2 uvscale;
    vec4 surfacecolor;//透明物体的表面颜色
};
layout(std140, binding = 1) uniform UniformMeshBuffer {
    UniformBlockMesh ubomeshdata;
};
layout (binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout (binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
layout (binding = 0, offset = 0) uniform atomic_uint list_counter;

layout(binding = 15)uniform CSM_TEX{
    sampler2D shadowmapTex[3];
};
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
layout(std140, binding = 0) uniform UniformBaseBuffer {
    UniformBlockBase ubobasedata;
};
struct PointLight{
    vec3 position;
    float constant;
    vec3 strength;
    float linear;
    float quadratic;
    float radius;
    float cutoff;
    float fill2;
};
layout (std430, binding = 0) buffer PointLightBuffer {
    PointLight pointLightData[POINT_LIGHT_COUNT];
};
layout (std430, binding = 1) readonly buffer visible_lights_indices {
    uint lights_indices[];
};
layout (std430, binding = 2) readonly buffer visible_lights_count {
    uint lights_count[];
};
in VS_OUT {
    vec3 worldpos;
    vec3 worldnormal;
    float cameraz;
} fs_in;
float getAttenuation(float radius, float cutoff, float distance){
    return smoothstep(radius, cutoff, distance);
}
void main() {
    uint index;//计数器值
    uint old_head;
    uvec4 item;//打包的数据
    vec4 frag_color;//片元颜色
    //----------------------------------------------------------------
    vec3 Lo = vec3(0);
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(TILE_SIZE, TILE_SIZE);
    uint index1 = tileID.y * ubobasedata.workgroup_x + tileID.x;
    uint lightcount = lights_count[index1];
    uint offset = index1 * TILE_LIGHT_MAX;
    //点光源
    for (int i = 0; i < lightcount; ++i)
    {
        uint lightindex = lights_indices[offset + i];
        // calculate per-light radiance
        vec3 L = normalize(pointLightData[lightindex].position - fs_in.worldpos);
        vec3 V = normalize(ubobasedata.camerapos-fs_in.worldpos);
        vec3 H = normalize(V + L);
        vec3 N = normalize(fs_in.worldnormal);
        float distance = length(pointLightData[lightindex].position - fs_in.worldpos);
        float attenuation = getAttenuation(pointLightData[lightindex].radius, pointLightData[lightindex].cutoff, distance);
        vec3 radiance = pointLightData[lightindex].strength * attenuation;

        float NdotL = dot(N, L);
        float NdotH = dot(N, H);
        float diff = max(dot(N, L), 0.1);
        float spec = pow(max(dot(N, H), 0.1), 32.0);
        Lo+=diff*radiance*ubomeshdata.surfacecolor.rgb+spec*radiance;
    }
    //平行光
    {
        vec3 L = ubobasedata.dirLights[0].direction;
        vec3 V = normalize(ubobasedata.camerapos-fs_in.worldpos);
        vec3 N = normalize(fs_in.worldnormal);
        vec3 H = normalize(L + V);
        float NdotL = dot(N, L);
        float NdotH = dot(N, H);
        float diff = max(dot(N, L), 0.1);
        float spec = pow(max(dot(N, H), 0.1), 32.0);
        Lo+=diff*ubobasedata.dirLights[0].strength*ubomeshdata.surfacecolor.rgb+spec*ubobasedata.dirLights[0].strength;
    }
    frag_color = vec4(Lo, ubomeshdata.surfacecolor.a);

    //----------------------------------------------------------------
    index = atomicCounterIncrement(list_counter);
    //将计数器值写入链表头图像中此片元对应的位置
    old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), index);
    item.x = old_head;
    item.y = packUnorm4x8(frag_color);
    item.z = floatBitsToUint(gl_FragCoord.z);
    item.w = 0;
    imageStore(list_buffer, int(index), item);
}