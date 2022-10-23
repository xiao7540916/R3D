#version 460
#extension GL_ARB_bindless_texture : require
layout (early_fragment_tests) in;
#define DIRECTION_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_SIZE 16
#define TILE_LIGHT_MAX 256
#define NUM_SAMPLES 16
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define EPS 1e-3
const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;
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
    //smoothstep(a,b,x) if(x<a);return 0；if(x>b);return 1;
    //t = clamp((x-a)/(b-a),0,1);return t*t(3-2*t);
    return 1.0f-smoothstep(cutoff, radius, distance);
}
//-----------------------------------------------------------
const float csmsplit[4] = { 0.0, 0.15, 0.45, 1.0 };//相机可视空间z方向分割比例
float csmdst[3];
float csmgddststart[3];
float csmgddstend[3];
float frustumSize[3] = { 21.666, 64.428, 142.8232 };//相机空间尺寸

const float shadowMapSize = 1024.0f;

float rand_2to1(vec2 uv) {
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(const in vec2 randomSeed) {

    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(randomSeed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < NUM_SAMPLES; i ++) {
        poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}
float ShadowCalculation(vec3 worldnormal, float filtersize)
{
    //获取层级
    int camindex;
    for (int i = 0; i < 3; ++i) {
        if (fs_in.cameraz>=csmgddststart[i]){
            camindex = i;
        }
    }
    //检查是否在过渡区域
    bool gdqy = false;
    if (fs_in.cameraz<csmgddstend[camindex]){
        gdqy = true;
    }

    if (gdqy){
        //计算层级占比
        float dstpart = (fs_in.cameraz-csmgddststart[camindex])/(csmgddstend[camindex]-csmgddststart[camindex]);
        float srcvisible = 0.0f;
        float dstvisible = 0.0f;
        {
            vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex-1]*vec4(fs_in.worldpos, 1.0);
            // 执行透视除法
            vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            // 取得当前片段在光源视角下的深度
            float currentDepth = projCoords.z;

            float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex-1]/(shadowMapSize*2.0);
            float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
            float sincita = sqrt(1-coscita*coscita);
            float tancita = min(1, sincita/coscita);
            float DepthBias = A*tancita;
            float NormalBias = A*sincita;

            float bias = ubobasedata.depthbias*DepthBias+ubobasedata.normalbias*NormalBias;

            //pfc
            vec2 texelSize = 1.0 / textureSize(shadowmapTex[camindex-1], 0);

            //采用泊松分布采样
            for (int i=0;i<NUM_SAMPLES;i++){
                float pcfDepth = texture(shadowmapTex[camindex-1], projCoords.xy + filtersize*texelSize*poissonDisk[i]).r;
                srcvisible += currentDepth - bias < pcfDepth ? 1.0 : 0.0;
            }
            srcvisible /= float(NUM_SAMPLES);
        }
        {
            vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex]*vec4(fs_in.worldpos, 1.0);
            // 执行透视除法
            vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            // 取得当前片段在光源视角下的深度
            float currentDepth = projCoords.z;

            float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex]/(shadowMapSize*2.0);
            float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
            float sincita = sqrt(1-coscita*coscita);
            float tancita = min(1, sincita/coscita);
            float DepthBias = A*tancita;
            float NormalBias = A*sincita;
            float bias = ubobasedata.depthbias*DepthBias+ubobasedata.normalbias*NormalBias;

            //pfc
            vec2 texelSize = 1.0 / textureSize(shadowmapTex[camindex], 0);

            //采用泊松分布采样
            for (int i=0;i<NUM_SAMPLES;i++){
                float pcfDepth = texture(shadowmapTex[camindex], projCoords.xy + filtersize*texelSize*poissonDisk[i]).r;
                dstvisible += currentDepth - bias < pcfDepth ? 1.0 : 0.0;
            }
            dstvisible /= float(NUM_SAMPLES);
        }
        float visible = srcvisible*(1.0f-dstpart)+dstvisible*dstpart;
        //如果超出可视范围，则没有阴影
        return visible;
    } else {
        float visible = 0.0f;
        vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex]*vec4(fs_in.worldpos, 1.0);
        // 执行透视除法
        vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
        // 变换到[0,1]的范围
        projCoords = projCoords * 0.5 + 0.5;
        // 取得当前片段在光源视角下的深度
        float currentDepth = projCoords.z;

        float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex]/(shadowMapSize*2.0);
        float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
        float sincita = sqrt(1-coscita*coscita);
        float tancita = min(1, sincita/coscita);
        float DepthBias = A*tancita;
        float NormalBias = A*sincita;
        float bias = ubobasedata.depthbias*DepthBias+ubobasedata.normalbias*NormalBias;

        //pfc
        vec2 texelSize = 1.0 / textureSize(shadowmapTex[camindex], 0);

        //采用泊松分布采样
        for (int i=0;i<NUM_SAMPLES;i++){
            float pcfDepth = texture(shadowmapTex[camindex], projCoords.xy + filtersize*texelSize*poissonDisk[i]).r;
            visible += currentDepth - bias < pcfDepth ? 1.0 : 0.0;
        }
        visible /= float(NUM_SAMPLES);
        //如果超出可视范围，则没有阴影
        return visible;
    }
}
//-----------------------------------------------------------
void main() {
    poissonDiskSamples(fs_in.worldpos.xz);//获得泊松分布采样点
    for (int i = 0; i < 3; ++i) {
        csmdst[i] = ubobasedata.znear+csmsplit[i]*(ubobasedata.zfar-ubobasedata.znear);
        csmgddststart[i] = csmdst[i]*0.9;
        csmgddstend[i] = csmdst[i]*1.1;
    }

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
    float specmax = 0.0f;
    vec3 V = normalize(ubobasedata.camerapos-fs_in.worldpos);
    vec3 N = normalize(fs_in.worldnormal);
    //点光源
    for (int i = 0; i < lightcount; ++i)
    {
        uint lightindex = lights_indices[offset + i];
        // calculate per-light radiance
        vec3 L = normalize(pointLightData[lightindex].position - fs_in.worldpos);
        vec3 H = normalize(V + L);
        float distance = length(pointLightData[lightindex].position - fs_in.worldpos);
        float attenuation = getAttenuation(pointLightData[lightindex].radius, pointLightData[lightindex].cutoff, distance);
        vec3 radiance = pointLightData[lightindex].strength * attenuation;

        float NdotL = dot(N, L);
        float NdotH = dot(N, H);
        float diff = max(dot(N, L), 0.1);
        float spec = pow(max(dot(N, H), 0.1), 64.0);
        specmax = max(spec, specmax);
        Lo+=diff*radiance*ubomeshdata.surfacecolor.rgb+spec*radiance;
    }
    //平行光
    {
        vec3 L = ubobasedata.dirLights[0].direction;
        vec3 H = normalize(L + V);
        float NdotL = dot(N, L);
        float NdotH = dot(N, H);
        float diff = max(dot(N, L), 0.1);
        float spec = pow(max(dot(N, H), 0.1), 64.0);
        specmax = max(spec, specmax);
        Lo+=diff*ubobasedata.dirLights[0].strength*ubomeshdata.surfacecolor.rgb+spec*ubobasedata.dirLights[0].strength;
    }
    float visible = ShadowCalculation(N, 3.0);
    vec3 ambient = ubobasedata.ambient*ubomeshdata.surfacecolor.rgb;
//    frag_color = vec4(Lo*visible+ambient, mix(ubomeshdata.surfacecolor.a, 0.96, specmax*0.9));
    frag_color = vec4(Lo*visible+ambient, ubomeshdata.surfacecolor.a);
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