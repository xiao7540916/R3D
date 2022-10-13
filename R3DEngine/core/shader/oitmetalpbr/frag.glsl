#version 460
#extension GL_ARB_bindless_texture : require
layout(early_fragment_tests) in;
#define DIRECTION_LIGHT_COUNT 4
#define POINT_LIGHT_COUNT 1024
#define TILE_SIZE 16
#define TILE_LIGHT_MAX 256
layout(location = 0)out vec4 FragColor;
layout(binding = 0)uniform sampler2D albedoTex;
layout(binding = 1)uniform sampler2D normalTex;
layout(binding = 2)uniform sampler2D metalTex;
layout(binding = 3)uniform sampler2D roughnessTex;
layout(binding = 4)uniform sampler2D aoTex;
layout(binding = 5)uniform sampler2D globalAoTex;
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
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    float CameraZ;
} fs_in;


#define NUM_SAMPLES 16
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define EPS 1e-3
const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;

vec3 GetNormalFromMap()
{
    vec3 normal = texture(normalTex, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    return normalize(fs_in.TBN*normal);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
float getAttenuation(float radius, float cutoff, float distance){
    return smoothstep(radius, cutoff, distance);
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
        if (fs_in.CameraZ>=csmgddststart[i]){
            camindex = i;
        }
    }
    //检查是否在过渡区域
    bool gdqy = false;
    if (fs_in.CameraZ<csmgddstend[camindex]){
        gdqy = true;
    }

    if (gdqy){
        //计算层级占比
        float dstpart = (fs_in.CameraZ-csmgddststart[camindex])/(csmgddstend[camindex]-csmgddststart[camindex]);
        float srcvisible = 0.0f;
        float dstvisible = 0.0f;
        {
            vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex-1]*vec4(fs_in.FragPos, 1.0);
            // 执行透视除法
            vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            // 取得当前片段在光源视角下的深度
            float currentDepth = projCoords.z;

            float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex-1]/(shadowMapSize*2.0);
            float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
            float sincita = sqrt(1-coscita*coscita);
            float tancita = min(1,sincita/coscita);
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
            vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex]*vec4(fs_in.FragPos, 1.0);
            // 执行透视除法
            vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            // 取得当前片段在光源视角下的深度
            float currentDepth = projCoords.z;

            float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex]/(shadowMapSize*2.0);
            float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
            float sincita = sqrt(1-coscita*coscita);
            float tancita = min(1,sincita/coscita);
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
        vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[camindex]*vec4(fs_in.FragPos, 1.0);
        // 执行透视除法
        vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
        // 变换到[0,1]的范围
        projCoords = projCoords * 0.5 + 0.5;
        // 取得当前片段在光源视角下的深度
        float currentDepth = projCoords.z;

        float A = (1.0f+float(ceil(filtersize)))*frustumSize[camindex]/(shadowMapSize*2.0);
        float coscita = clamp(dot(normalize(worldnormal), ubobasedata.dirLights[0].direction), 0.0, 1.0);
        float sincita = sqrt(1-coscita*coscita);
        float tancita = min(1,sincita/coscita);
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
    poissonDiskSamples(fs_in.FragPos.xz);//获得泊松分布采样点
//    poissonDisk[0] = vec2(0);
    for (int i = 0; i < 3; ++i) {
        csmdst[i] = ubobasedata.znear+csmsplit[i]*(ubobasedata.zfar-ubobasedata.znear);
        csmgddststart[i] = csmdst[i]*0.9;
        csmgddstend[i] = csmdst[i]*1.1;
    }
    vec4 fragPosLightSpace = ubobasedata.lightviewprojdata[0]*vec4(fs_in.FragPos, 1.0);
    float globalao = texture(globalAoTex, vec2(gl_FragCoord.xy)/vec2(ubobasedata.windowwidth, ubobasedata.windowheight)).r;

    vec3 albedo     = pow(texture(albedoTex, fs_in.TexCoords).rgb, vec3(2.2));//转到线性空间
    float metallic  = texture(metalTex, fs_in.TexCoords).r;
    float roughness = texture(roughnessTex, fs_in.TexCoords).r;
    float ao        = texture(aoTex, fs_in.TexCoords).r;

    vec3 camPos = ubobasedata.camerapos;
    vec3 WorldPos = fs_in.FragPos;
    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    float visible = ShadowCalculation(N, 3.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(TILE_SIZE, TILE_SIZE);
    uint index = tileID.y * ubobasedata.workgroup_x + tileID.x;
    uint lightcount = lights_count[index];
    uint offset = index * TILE_LIGHT_MAX;
    //点光源
    for (int i = 0; i < lightcount; ++i)
    {
        uint lightindex = lights_indices[offset + i];
        // calculate per-light radiance
        vec3 L = normalize(pointLightData[lightindex].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLightData[lightindex].position - WorldPos);
        float attenuation = getAttenuation(pointLightData[lightindex].radius, pointLightData[lightindex].cutoff, distance);
        vec3 radiance = pointLightData[lightindex].strength * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;// 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    //方向光
    for (int i = 0; i < ubobasedata.dirlightactivenum; ++i)
    {
        // calculate per-light radiance
        vec3 L = ubobasedata.dirLights[i].direction;
        vec3 H = normalize(V + L);
        vec3 radiance = ubobasedata.dirLights[i].strength;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;// 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += visible*(kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }


    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.01) * albedo * ao * globalao;

    vec3 color = ambient + Lo;
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
//    FragColor = vec4(vec3(visible),1.0);
}
